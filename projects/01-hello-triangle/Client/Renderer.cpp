#include "Renderer.h"
#include "Scene.h"
#include "sys.h"
#include "ResourceLib.h"
#include "Shader.h"
#include "TextureCube.h"
#include "camera.h"

Renderer::Renderer()
{
	m_shadowMap = std::make_unique<ShadowMap>(1024, 1024);
	m_framebuffer = std::make_unique<Framebuffer>(800, 800);
	float quadVerts[] = {  // pos2 + uv2
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,
	};
	glGenVertexArrays(1, &m_quadVAO);
	glGenBuffers(1, &m_quadVBO);
	glBindVertexArray(m_quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glBindVertexArray(0);

}

Renderer::~Renderer()
{
	glDeleteBuffers(1, &m_quadVBO);
	glDeleteVertexArrays(1, &m_quadVAO);
}

void Renderer::RenderFrame(Scene& scene, int fbw, int fbh)
{
	scene.shadowInfo().enabled = false;
	RenderShadowMap(scene);
	m_framebuffer->Begin(fbw, fbh);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene.render();
	RenderSkybox(scene);

	m_framebuffer->End();
	RenderPostEffect();
	//Framebuffer::BlitTo(m_framebuffer.get(), nullptr);

	//DrawDebugDepth();
}


void Renderer::RenderShadowMap(Scene& scene)
{
	LightComponent* shadowLight = nullptr;
	for (LightComponent* l : scene.collectLights())
	{
		if (l->castShadow) { shadowLight = l; break; }
	}
	if (!shadowLight) return;
	glm::mat4 lightProj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 10.0f);
	glm::mat4 lightView = glm::lookAt(
		shadowLight->GetPos(),
		shadowLight->GetPos() + shadowLight->direction,
		glm::vec3(0.0f, 0.0f, -1.0f)
	);
	glm::mat4 lightSpaceMatrix = lightProj * lightView;
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	m_shadowMap->Begin();
	{
		Shader* s = ResourceLib::GetShader(
			(getAssetPath() + "shaders/shadowMap.vert").c_str(),
			(getAssetPath() + "shaders/shadowMap.frag").c_str());
		if (!s) { m_shadowMap->End(); return; }

		s->UseProgram();
		s->setUniformMat4("lightSpaceMatrix", lightSpaceMatrix);
		scene.renderDepth(lightSpaceMatrix);
	}
	glDisable(GL_CULL_FACE);
	m_shadowMap->End();
	ShadowFrame& sh = scene.shadowInfo();
	sh.enabled = true;
	sh.lightSpaceMat = lightSpaceMatrix;
	sh.depthTex = m_shadowMap->GetDepthTexture();
	sh.lightDir = shadowLight->direction;
}

void Renderer::DrawDebugDepth()
{
	Shader* s = ResourceLib::GetShader(
		(getAssetPath() + "shaders/debugQuad.vert").c_str(),
		(getAssetPath() + "shaders/debugQuad.frag").c_str()
	);
	if (!s) return;

	s->UseProgram();
	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_shadowMap->GetDepthTexture());
	s->setUniform1i("depthMap", 0);
	glBindVertexArray(m_quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::RenderSkybox(Scene& scene)
{
	TextureCube* sky = scene.skybox();
	if (!sky || !sky->IsValid()) return;

	// 天空盒 pass 自依赖的状态自己设，不信任别的 pass 留下的
	glDepthFunc(GL_LEQUAL);   // 深度被钉在 1.0，默认 LESS 会被远平面自己挡掉
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);     // 相机在盒子内部，看得见的是"内表面"

	Shader* s = ResourceLib::GetShader(
		(getAssetPath() + "shaders/skybox.vert").c_str(),
		(getAssetPath() + "shaders/skybox.frag").c_str());
	Mesh* cube = ResourceLib::GetMesh("builtin:cube");
	CameraComponent* cam = scene.mainCamera();
	if (s && cube && cam)
	{
		s->UseProgram();
		s->setUniformMat4("view", cam->getCameraView());
		s->setUniformMat4("projection", cam->getProjection());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, sky->GetID());
		glUniform1i(glGetUniformLocation(s->getProgram(), "skybox"), 0);

		cube->draw();
	}

	glDepthFunc(GL_LESS);
	glDisable(GL_CULL_FACE);
}

void Renderer::RenderPostEffect()
{
	Shader* s = ResourceLib::GetShader(
		(getAssetPath() + "shaders/post.vert").c_str(),
		(getAssetPath() + "shaders/post.frag").c_str());
	if (!s) return;

	s->UseProgram();
	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_framebuffer->GetColorTexture());
	glUniform1i(glGetUniformLocation(s->getProgram(), "screenTex"), 0);
	glUniform1i(glGetUniformLocation(s->getProgram(), "effectMode"), m_effectMode);

	glBindVertexArray(m_quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glEnable(GL_DEPTH_TEST);
}