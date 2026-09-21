#include "ResourceLib.h"
#include "sys.h"
#include <cstring>

static std::unordered_map<std::string, Mesh*>& MeshCache()
{
	static std::unordered_map<std::string, Mesh*> c; return c;
}
static std::unordered_map<std::string, Shader*>& ShaderCache()
{
	static std::unordered_map<std::string, Shader*> c; return c;
}
static std::unordered_map<std::string, unsigned int>& TexCache()
{
	static std::unordered_map<std::string, unsigned int> c; return c;
}

Mesh* ResourceLib::GetMesh(const char* key)
{
	auto it = MeshCache().find(key);
	if (it != MeshCache().end()) return it->second;

	Mesh* m = nullptr;
	if (strcmp(key, "builtin:cube") == 0)        m = Mesh::CreateCube();
	else if (strcmp(key, "builtin:ground") == 0) m = Mesh::CreateGround();
	else m = Mesh::LoadOBJ((getAssetPath() + key).c_str());

	if (m) { MeshCache()[key] = m; LOG_INFO("Mesh first load: %s", key); }
	return m;
}

Shader* ResourceLib::GetShader(const char* vert, const char* frag)
{
	std::string key = std::string(vert) + "|" + frag;
	auto it = ShaderCache().find(key);
	if (it != ShaderCache().end()) return it->second;

	Shader* s = new Shader(vert, frag);
	ShaderCache()[key] = s;
	LOG_INFO("Shader first compile: %s", key.c_str());

	return s;
}

unsigned int ResourceLib::GetTexture(const char* path)
{
	auto it = TexCache().find(path);
	if (it != TexCache().end()) return it->second;

	unsigned int t = Material::LoadTexture(path);
	TexCache()[path] = t;
	LOG_INFO("Texture 首次加载: %s", path);
	return t;
}

void ResourceLib::Shutdown()
{
	for (auto& kv : MeshCache()) delete kv.second;
	MeshCache().clear();
	for (auto& kv : ShaderCache()) delete kv.second;
	ShaderCache().clear();
	for (auto& kv : TexCache()) glDeleteTextures(1, &kv.second);
	TexCache().clear();
}






