#pragma once
#include "pch.h"
#include "mesh.h"
#include "Material.h"

class TextureCube;

class ResourceLib
{
public:
	static Mesh* GetMesh(const char* key);
	static Material* GetMaterial(const char* key);
	static Shader* GetShader(const char* vert, const char* frag);
	static unsigned int GetTexture(const char* path);
	static TextureCube* GetSkybox(const char* dir);
	static void Shutdown();
};