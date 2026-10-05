#include "render/RendererAPI.h"
#include "render/OpenGLRendererAPI.h"

RendererAPI* RendererAPI::Create()
{
	return new OpenGLRendererAPI();
}