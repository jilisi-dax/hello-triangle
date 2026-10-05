#include "render/Renderer.h"
#include "scene/Scene.h"
#include "scene/LightComponent.h"
#include "platform/Paths.h"
#include "render/ResourceLib.h"
#include "render/Shader.h"
#include "render/TextureCube.h"
#include "scene/components/CameraComponent.h"
#include "render/OpenGLRendererAPI.h"

Renderer::Renderer()
{
	m_api.reset(RendererAPI::Create());
	m_api->Init();
	m_shadowMap.reset(Framebuffer::Create(1024, 1024, true));
	m_framebuffer.reset(Framebuffer::Create(800, 800));
	m_bloomA.reset(Framebuffer::Create(800, 800));
	m_bloomB.reset(Framebuffer::Create(800, 800));
	for (int i = 0; i < 4; i++)
		m_bloomMip[i].reset(Framebuffer::Create(800 >> (i + 1), 800 >> (i + 1)));
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
	m_api->SetDepthTest(true);
	m_api->SetCullFace(true, CullMode::Back);
	m_api->SetClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	m_api->ClearColorDepth();
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
	m_api->SetDepthTest(true);
	m_api->SetCullFace(true, CullMode::Front);
	m_shadowMap->Begin(1024, 1024);
	{
		Shader* s = ResourceLib::GetShader(
			(getAssetPath() + "shaders/shadowMap.vert").c_str(),
			(getAssetPath() + "shaders/shadowMap.frag").c_str());
		if (!s) { m_shadowMap->End(); return; }

		s->UseProgram();
		s->setUniformMat4("lightSpaceMatrix", lightSpaceMatrix);
		scene.renderDepth(lightSpaceMatrix);
	}
	m_api->SetCullFace(false);
	m_shadowMap->End();
	ShadowFrame& sh = scene.shadowInfo();
	sh.enabled = true;
	sh.lightSpaceMat = lightSpaceMatrix;
	sh.shadowRT = m_shadowMap.get();
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
	m_api->SetDepthTest(false);
	glActiveTexture(GL_TEXTURE0);
	m_shadowMap->BindDepth(0);
	s->setUniform1i("depthMap", 0);
	glBindVertexArray(m_quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::RenderSkybox(Scene& scene)
{
	TextureCube* sky = scene.skybox();
	if (!sky || !sky->IsValid()) return;

	// 天空盒 pass 自依赖的状态自己设，不信任别的 pass 留下的
	m_api->SetDepthFunc(DepthFunc::LEqual);   // 深度被钉在 1.0，默认 LESS 会被远平面自己挡掉
	m_api->SetCullFace(true, CullMode::Front);		 // 相机在盒子内部，看得见的是"内表面"

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
		if (m_debugEnv == 1 && scene.env().irradiance)
			scene.env().irradiance->Bind(0);
		else if
			(m_debugEnv == 2 && scene.env().prefiltered) scene.env().prefiltered->Bind(0);
		else
			sky->Bind(0); //用天空盒当debug画板？
		s->setUniform1i("skybox", 0);
		cube->draw();
	}

	m_api->SetDepthFunc(DepthFunc::Less);
	m_api->SetCullFace(false);
}

void Renderer::RenderPostEffect(int w, int h)
{
	Shader* s = ResourceLib::GetShader(
		(getAssetPath() + "shaders/post.vert").c_str(),
		(getAssetPath() + "shaders/post.frag").c_str());
	if (!s) return;

	s->UseProgram();
	m_api->SetViewport(0, 0, w, h);
	m_api->SetDepthTest(false);
	glActiveTexture(GL_TEXTURE0);
	m_framebuffer->BindColor(0);
	s->setUniform1i("screenTex", 0);
	s->setUniform1i("effectMode", m_effectMode);

	glActiveTexture(GL_TEXTURE1);
	m_bloomMip[2]->BindColor(1);
	s->setUniform1i("bloomTex", 1);

	glBindVertexArray(m_quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	m_api->SetDepthTest(true);
}

void Renderer::RenderBloomExtract(int w, int h)
{
	Shader* s = ResourceLib::GetShader(
		(getAssetPath() + "shaders/post.vert").c_str(),
		(getAssetPath() + "shaders/brightExtract.frag").c_str());
	if (!s) return;

	m_bloomA->Begin(w, h);
	s->UseProgram();
	m_api->SetDepthTest(false);
	glActiveTexture(GL_TEXTURE0);
	m_framebuffer->BindColor(0);
	s->setUniform1i("screenTex", 0);
	glBindVertexArray(m_quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	m_bloomA->End();
	m_api->SetDepthTest(true);

}

void Renderer::RenderBloomDown(int w, int h)
{
	Shader* s = ResourceLib::GetShader(
		(getAssetPath() + "shaders/post.vert").c_str(),
		(getAssetPath() + "shaders/util_down.frag").c_str());
	if (!s) return;

	s->UseProgram();
	m_api->SetDepthTest(false);
	glBindVertexArray(m_quadVAO);
	s->setUniform1i("screenTex", 0);
	glActiveTexture(GL_TEXTURE0);
	for (int i = 0; i < 4; i++)
	{
		Framebuffer* src = (i == 0) ? m_bloomA.get() : m_bloomMip[i - 1].get();
		src->BindColor(0);
		m_bloomMip[i]->Begin(w >> (i + 1), h >> (i + 1));
		glDrawArrays(GL_TRIANGLES, 0, 6);
		m_bloomMip[i]->End();
	}
	m_api->SetDepthTest(true);
}
void Renderer::RenderBloomUp(int w, int h)
{
	Shader* s = ResourceLib::GetShader(
		(getAssetPath() + "shaders/post.vert").c_str(),
		(getAssetPath() + "shaders/util_up.frag").c_str());
	if (!s) return;

	s->UseProgram();
	m_api->SetDepthTest(false);
	glBindVertexArray(m_quadVAO);
	s->setUniform1i("screenTex", 0);
	glActiveTexture(GL_TEXTURE0);

	m_api->SetBlend(true);
	m_api->SetBlendFunc(BlendFactor::One, BlendFactor::One);

	for (int i = 3; i > 0; i--)
	{
		m_bloomMip[i]->BindColor(0);
		m_bloomMip[i - 1]->Begin(w >> i, h >> i);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		m_bloomMip[i - 1]->End();
	}

	m_bloomMip[0]->BindColor(0);
	m_bloomA->Begin(w, h);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	m_bloomA->End();

	m_api->SetBlend(false);
	m_api->SetDepthTest(true);
}