#include "scene/components/MeshRendererComponent.h"
#include "scene/LightComponent.h"
#include "scene/components/CameraComponent.h"
#include "platform/Paths.h"

using namespace std;

void MeshRendererComponent::draw(CameraComponent& cam, std::vector<LightComponent*>& lights, const ShadowFrame& shadow, const EnvFrame& env)
{
	if (!m_material || !m_mesh) return;
	m_material->bind();
	Shader* s = m_material->m_shader;
	unsigned int prog = s->getProgram();

	glUniformMatrix4fv(s->loc("view"), 1, GL_FALSE, glm::value_ptr(cam.getCameraView()));
	glUniformMatrix4fv(s->loc("projection"), 1, GL_FALSE, glm::value_ptr(cam.getProjection()));
	glUniform3f(s->loc("viewPos"),cam.cameraPos.x, cam.cameraPos.y, cam.cameraPos.z);

	glm::mat4 model = owner ? owner->getModelMatrix() : glm::mat4(1.0f);
	glUniformMatrix4fv(s->loc("model"), 1, GL_FALSE, glm::value_ptr(model));

	glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(model)));
	glUniformMatrix3fv(s->loc("normalMatrix"),
		1, GL_FALSE, glm::value_ptr(normalMat));

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
				glUniform3f(s->loc(("lightPos[" + std:: to_string(count) + "]").c_str()),
					lights[i]->GetPos().x, lights[i]->GetPos().y, lights[i]->GetPos().z);
				glUniform3f(s->loc(("lightColor[" + to_string(count) + "]").c_str()),
					lights[i]->lightColor.x, lights[i]->lightColor.y, lights[i]->lightColor.z);
				glUniform3f(s->loc(("spotDir[" + to_string(count) + "]").c_str()),
					lights[i]->direction.x, lights[i]->direction.y, lights[i]->direction.z);
				glUniform1f(s->loc(("spotCutoff[" + to_string(count) + "]").c_str()),
					lights[i]->cutoff);
				glUniform1f(s->loc(("spotCutoffOuter[" + to_string(count) + "]").c_str()),
					lights[i]->cutoffOuter);
				glUniform1f(s->loc(("lightRange[" + to_string(count) + "]").c_str()),
					lights[i]->range);
				glUniform1i(s->loc(("shadowFlags[" + to_string(count) + "]").c_str()),
					lights[i]->castShadow ? 1 : 0);
				count++;
			}
		}

	}
	glUniform1i(s->loc("lightCount"), count);
	glUniform1f(s->loc("time"), (float)glfwGetTime());

	if (shadow.enabled)
	{
		glActiveTexture(GL_TEXTURE1);   // 0 号材质贴图用，深度图挂 1 号
		glBindTexture(GL_TEXTURE_2D, shadow.depthTex);
		glUniform1i(s->loc("shadowMap"), 1);
		glUniformMatrix4fv(s->loc("lightSpaceMatrix"),
			1, GL_FALSE, glm::value_ptr(shadow.lightSpaceMat));
		glUniform3f(s->loc("shadowLightDir"),
			shadow.lightDir.x, shadow.lightDir.y, shadow.lightDir.z);
	}
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, env.irradianceTex);
	glUniform1i(s->loc("irradianceMap"), 5);
	glUniform1i(s->loc("irradianceEnabled"), env.enabled ? 1 : 0);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_CUBE_MAP, env.prefilteredTex);
	glUniform1i(s->loc("prefilterMap"), 6);
	glUniform1i(s->loc("prefilteredEnabled"), env.enabled ? 1 : 0);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, env.brdfLutTex);
	glUniform1i(s->loc("brdfLut"), 7);
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