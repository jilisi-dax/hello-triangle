#pragma once
#include "pch.h"
#include "Mesh.h"
#include "Material.h"

class ResourceLib
{
public:
	static Mesh* GetMesh(const char* key);
	static Shader* GetShader(const char* vert, const char* frag);
	static unsigned int GetTexture(const char* path);
	static void Shutdown();
};