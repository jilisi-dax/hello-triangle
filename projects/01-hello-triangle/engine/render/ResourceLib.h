#pragma once
#include "core/pch.h"
#include "render/Mesh.h"
#include "render/Material.h"

class TextureCube;

class ResourceLib
{
public:
	static Mesh* GetMesh(const char* key);
	static Material* GetMaterial(const char* key);
	static Shader* GetShader(const char* vert, const char* frag);
	static unsigned int GetTexture(const char* path);
	static TextureCube* GetSkybox(const char* dir);
	static TextureCube* GetIrradiance(const char* dir);
	static TextureCube* GetPrefiltered(const char* dir);
	static unsigned int GetBrdfLut();
	static void Shutdown();
};