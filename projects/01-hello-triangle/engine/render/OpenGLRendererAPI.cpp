#include "render/OpenGLRendererAPI.h"
#include <glad/glad.h>

void OpenGLRendererAPI::Init()
{
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void OpenGLRendererAPI::SetViewport(int x, int y, int w, int h)
{
	glViewport(x, y, w, h);
}

void OpenGLRendererAPI::SetClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}

void OpenGLRendererAPI::ClearColorDepth()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRendererAPI::SetDepthTest(bool on)
{
	if (on)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}
void OpenGLRendererAPI::SetDepthFunc(DepthFunc f)
{
	switch (f)
	{
	case DepthFunc::Less:   
		glDepthFunc(GL_LESS);   
		break;
	case DepthFunc::LEqual: 
		glDepthFunc(GL_LEQUAL); 
		break;
	}
}

void OpenGLRendererAPI::SetCullFace(bool on, CullMode mode)
{
	if (!on) { glDisable(GL_CULL_FACE); return; }
	glEnable(GL_CULL_FACE);
	glCullFace(mode == CullMode::Back ? GL_BACK : GL_FRONT);
}

void OpenGLRendererAPI::SetBlend(bool on)
{
	if (on) glEnable(GL_BLEND);
	else    glDisable(GL_BLEND);
}

void OpenGLRendererAPI::SetBlendFunc(BlendFactor src, BlendFactor dst)
{
	auto toGL = [](BlendFactor f) {
		switch (f)
		{
		case BlendFactor::One:
			return GL_ONE;
		case BlendFactor::SrcAlpha:
			return GL_SRC_ALPHA;
		case BlendFactor::OneMinusSrcAlpha:
			return GL_ONE_MINUS_SRC_ALPHA;
		}
		return GL_ONE;
		};
	glBlendFunc(toGL(src), toGL(dst));
}

