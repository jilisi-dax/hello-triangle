#pragma once
#include "core/pch.h"
#include "render/Mesh.h"
#include "render/Material.h"

class TextureCube;
class Texture2D;

class ResourceLib
{
public:
	static Mesh* GetMesh(const char* key);
	static Material* GetMaterial(const char* key);
	static Shader* GetShader(const char* vert, const char* frag);
	static Texture2D* GetTexture(const char* path);
	static TextureCube* GetSkybox(const char* dir);
	static TextureCube* GetIrradiance(const char* dir);
	static TextureCube* GetPrefiltered(const char* dir);
	static Texture2D* GetBrdfLut();
	static void Shutdown();
};