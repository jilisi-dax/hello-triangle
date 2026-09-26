#include "renderComponent.h"
#include "camera.h"
#include "sys.h"

using namespace std;

void MeshRendererComponent::draw(CameraComponent& cam, std::vector<LightComponent*>& lights, const ShadowFrame& shadow)
{
	if (!m_material || !m_mesh) return;
	m_material->bind();
	unsigned int prog = m_material->m_shader->getProgram();

	glUniformMatrix4fv(glGetUniformLocation(prog, "view"), 1, GL_FALSE, glm::value_ptr(cam.getCameraView()));
	glUniformMatrix4fv(glGetUniformLocation(prog, "projection"), 1, GL_FALSE, glm::value_ptr(cam.getProjection()));
	glUniform3f(glGetUniformLocation(prog, "viewPos"),cam.cameraPos.x, cam.cameraPos.y, cam.cameraPos.z);

	glm::mat4 model = owner ? owner->getModelMatrix() : glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(prog, "model"), 1, GL_FALSE, glm::value_ptr(model));

	glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(model)));
	glUniformMatrix3fv(glGetUniformLocation(prog, "normalMatrix"),
		1, GL_FALSE, glm::value_ptr(normalMat));

	// 用收集到的灯
	int count = 0;
	if (!lights.empty())
	{
		glm::vec3 objPos = owner ? owner->GetPos() : glm::vec3(0.0f);
		for (int i = 0; i < lights.size() && count < 8; i++)
		{
			float dx = lights[i]->GetPos().x - objPos.x;
			float dy = lights[i]->GetPos().y - objPos.y;
			float dz = lights[i]->GetPos().z - objPos.z;
			float dist = sqrt(dx * dx + dy * dy + dz * dz);

			if (dist < lights[i]->range)
			{
				glUniform3f(glGetUniformLocation(prog,
					("lightPos[" + std:: to_string(count) + "]").c_str()),
					lights[i]->GetPos().x, lights[i]->GetPos().y, lights[i]->GetPos().z);

				glUniform3f(glGetUniformLocation(prog,
					("lightColor[" + to_string(count) + "]").c_str()),
					lights[i]->lightColor.x, lights[i]->lightColor.y, lights[i]->lightColor.z);
				glUniform3f(glGetUniformLocation(prog,
					("spotDir[" + to_string(count) + "]").c_str()),
					lights[i]->direction.x, lights[i]->direction.y, lights[i]->direction.z);
				glUniform1f(glGetUniformLocation(prog,
					("spotCutoff[" + to_string(count) + "]").c_str()),
					lights[i]->cutoff);
				glUniform1f(glGetUniformLocation(prog,
					("spotCutoffOuter[" + to_string(count) + "]").c_str()),
					lights[i]->cutoffOuter);
				count++;
			}
		}

	}
	glUniform1i(glGetUniformLocation(prog, "lightCount"), count);
	glUniform1f(glGetUniformLocation(prog, "time"), (float)glfwGetTime());

	if (shadow.enabled)
	{
		glActiveTexture(GL_TEXTURE1);   // 0 号材质贴图用，深度图挂 1 号
		glBindTexture(GL_TEXTURE_2D, shadow.depthTex);
		glUniform1i(glGetUniformLocation(prog, "shadowMap"), 1);
		glUniformMatrix4fv(glGetUniformLocation(prog, "lightSpaceMatrix"),
			1, GL_FALSE, glm::value_ptr(shadow.lightSpaceMat));
		glUniform3f(glGetUniformLocation(prog, "shadowLightDir"),
			shadow.lightDir.x, shadow.lightDir.y, shadow.lightDir.z);
	}
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