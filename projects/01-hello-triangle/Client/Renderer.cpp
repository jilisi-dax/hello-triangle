#include "Renderer.h"
#include "Scene.h"
#include "sys.h"
#include "ResourceLib.h"
#include "Shader.h"
#include "TextureCube.h"
#include "camera.h"

Renderer::Renderer()
{
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	m_shadowMap = std::make_unique<ShadowMap>(1024, 1024);
	m_framebuffer = std::make_unique<Framebuffer>(800, 800);
	m_bloomA = std::make_unique<Framebuffer>(800, 800);
	m_bloomB = std::make_unique<Framebuffer>(800, 800);
	for (int i = 0; i < 4; i++)
		m_bloomMip[i] = std::make_unique<Framebuffer>(800 >> (i + 1), 800 >> (i + 1));
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
	RenderBloomExtract(fbw, fbh);
	RenderBloomDown(fbw, fbh);
	RenderBloomUp(fbw, fbh);
	RenderPostEffect(fbw, fbh);
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
		unsigned int showId = sky->GetID();
		if (m_debugEnv == 1 && scene.env().irradianceTex) showId = scene.env().irradianceTex;
		if (m_debugEnv == 2 && scene.env().prefilteredTex) showId = scene.env().prefilteredTex;
		glBindTexture(GL_TEXTURE_CUBE_MAP, showId); //用天空盒当debug画板？
		glUniform1i(glGetUniformLocation(s->getProgram(), "skybox"), 0);
		cube->draw();
	}

	glDepthFunc(GL_LESS);
	glDisable(GL_CULL_FACE);
}

void Renderer::RenderPostEffect(int w, int h)
{
	Shader* s = ResourceLib::GetShader(
		(getAssetPath() + "shaders/post.vert").c_str(),
		(getAssetPath() + "shaders/post.frag").c_str());
	if (!s) return;

	s->UseProgram();
	glViewport(0, 0, w, h);
	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_framebuffer->GetColorTexture());
	glUniform1i(glGetUniformLocation(s->getProgram(), "screenTex"), 0);
	glUniform1i(glGetUniformLocation(s->getProgram(), "effectMode"), m_effectMode);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_bloomA->GetColorTexture());
	glUniform1i(glGetUniformLocation(s->getProgram(), "bloomTex"), 1);

	glBindVertexArray(m_quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::RenderBloomExtract(int w, int h)
{
	Shader* s = ResourceLib::GetShader(
		(getAssetPath() + "shaders/post.vert").c_str(),
		(getAssetPath() + "shaders/brightExtract.frag").c_str());
	if (!s) return;

	m_bloomA->Begin(w, h);
	s->UseProgram();
	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_framebuffer->GetColorTexture());
	glUniform1i(glGetUniformLocation(s->getProgram(), "screenTex"), 0);
	glBindVertexArray(m_quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	m_bloomA->End();
	glEnable(GL_DEPTH_TEST);

}

void Renderer::RenderBloomDown(int w, int h)
{
	Shader* s = ResourceLib::GetShader(
		(getAssetPath() + "shaders/post.vert").c_str(),
		(getAssetPath() + "shaders/util_down.frag").c_str());
	if (!s) return;

	s->UseProgram();
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(m_quadVAO);
	glUniform1i(s->loc("screenTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	for (int i = 0; i < 4; i++)
	{
		unsigned int src = (i == 0) ? m_bloomA->GetColorTexture()
			: m_bloomMip[i - 1]->GetColorTexture();
		glBindTexture(GL_TEXTURE_2D, src);
		m_bloomMip[i]->Begin(w >> (i + 1), h >> (i + 1));
		glDrawArrays(GL_TRIANGLES, 0, 6);
		m_bloomMip[i]->End();
	}
	glEnable(GL_DEPTH_TEST);
}
void Renderer::RenderBloomUp(int w, int h)
{
	Shader* s = ResourceLib::GetShader(
		(getAssetPath() + "shaders/post.vert").c_str(),
		(getAssetPath() + "shaders/util_up.frag").c_str());
	if (!s) return;

	s->UseProgram();
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(m_quadVAO);
	glUniform1i(s->loc("screenTex"), 0);
	glActiveTexture(GL_TEXTURE0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	for (int i = 3; i > 0; i--)
	{
		glBindTexture(GL_TEXTURE_2D, m_bloomMip[i]->GetColorTexture());
		m_bloomMip[i - 1]->Begin(w >> i, h >> i);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		m_bloomMip[i - 1]->End();
	}

	glBindTexture(GL_TEXTURE_2D, m_bloomMip[0]->GetColorTexture());
	m_bloomA->Begin(w, h);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	m_bloomA->End();

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}