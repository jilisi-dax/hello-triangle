#include "scene/components/MeshRendererComponent.h"
#include "scene/LightComponent.h"
#include "scene/components/CameraComponent.h"
#include "platform/Paths.h"
#include "platform/Time.h"
#include "render/Framebuffer.h"
#include "render/TextureCube.h"
#include "render/Texture2D.h"

using namespace std;

void MeshRendererComponent::draw(CameraComponent& cam, std::vector<LightComponent*>& lights, const ShadowFrame& shadow, const EnvFrame& env)
{
	if (!m_material || !m_mesh) return;
	m_material->bind();
	Shader* s = m_material->m_shader;

	s->setUniformMat4("view", cam.getCameraView());
	s->setUniformMat4("projection", cam.getProjection());
	s->setUniform3f("viewPos", cam.cameraPos.x, cam.cameraPos.y, cam.cameraPos.z);

	glm::mat4 model = owner ? owner->getModelMatrix() : glm::mat4(1.0f);
	s->setUniformMat4("model", model);

	glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(model)));
	s->setUniformMat3("normalMatrix", normalMat);

	// 用收集到的灯
	int count = 0;
	if (!lights.empty())
	{
		glm::vec3 objPos = owner ? owner->GetPos() : glm::vec3(0.0f);
		int n = (int)lights.size();
		if (n > 8) n = 8;
		for (int i = 0; i < n; i++)
		{
			float dx = lights[i]->GetPos().x - objPos.x;
			float dy = lights[i]->GetPos().y - objPos.y;
			float dz = lights[i]->GetPos().z - objPos.z;
			float dist = sqrt(dx * dx + dy * dy + dz * dz);

			if (dist < lights[i]->range)
			{
				s->setUniform3f(("lightPos[" + std:: to_string(count) + "]").c_str(), lights[i]->GetPos().x, lights[i]->GetPos().y, lights[i]->GetPos().z);
				s->setUniform3f(("lightColor[" + to_string(count) + "]").c_str(), lights[i]->lightColor.x, lights[i]->lightColor.y, lights[i]->lightColor.z);
				s->setUniform3f(("spotDir[" + to_string(count) + "]").c_str(), lights[i]->direction.x, lights[i]->direction.y, lights[i]->direction.z);
				s->setUniform1f(("spotCutoff[" + to_string(count) + "]").c_str(), lights[i]->cutoff);
				s->setUniform1f(("spotCutoffOuter[" + to_string(count) + "]").c_str(), lights[i]->cutoffOuter);
				s->setUniform1f(("lightRange[" + to_string(count) + "]").c_str(), lights[i]->range);
				s->setUniform1i(("shadowFlags[" + to_string(count) + "]").c_str(), lights[i]->castShadow ? 1 : 0);
				count++;
			}
		}
	}
	s->setUniform1i("lightCount", count);
	s->setUniform1f("time", Time::Now());
	if (shadow.enabled)
	{
		if (shadow.shadowRT) shadow.shadowRT->BindDepth(1);   // 0 号材质贴图用，深度图挂 1 号

		s->setUniform1i("shadowMap", 1);
		s->setUniformMat4("lightSpaceMatrix", shadow.lightSpaceMat);
		s->setUniform3f("shadowLightDir", shadow.lightDir.x, shadow.lightDir.y, shadow.lightDir.z);
	}
	if (env.irradiance) env.irradiance->Bind(5);
	s->setUniform1i("irradianceMap", 5);
	s->setUniform1i("irradianceEnabled", env.enabled ? 1 : 0);
	if (env.prefiltered) env.prefiltered->Bind(6);
	s->setUniform1i("prefilterMap", 6);
	s->setUniform1i("prefilteredEnabled", env.enabled ? 1 : 0);
	if (env.brdfLut) env.brdfLut->Bind(7);
	s->setUniform1i("brdfLut", 7);
	if (m_mesh) m_mesh->draw();
}

void MeshRendererComponent::drawDepth(const glm::mat4& lightSpaceMat)
{
	if (!m_mesh) return;

	Shader* s = ResourceLib::GetShader(
		(getAssetPath() + "shaders/shadowMap.vert").c_str(),
		(getAssetPath() + "shaders/shadowMap.frag").c_str());
	if (!s) return;

	s->UseProgram();
	s->setUniformMat4("model", owner ? owner->getModelMatrix() : glm::mat4(1.0f));
	m_mesh->draw();
}