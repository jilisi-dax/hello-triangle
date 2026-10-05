#pragma once
#include "render/Texture.h"

class TextureCube : public Texture
{
public:
	static TextureCube* Create(const char* const faces[6]);
	static TextureCube* CreateFromEquirect(const char* equirectPath);
	static TextureCube* CreateIrradiance(const TextureCube& env, int faceSize = 32);
	static TextureCube* CreatePrefiltered(const TextureCube& env, int faceSize = 128);
};