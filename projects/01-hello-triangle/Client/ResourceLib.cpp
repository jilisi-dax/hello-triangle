#include "ResourceLib.h"
#include "sys.h"
#include <cstring>

static std::unordered_map<std::string, std::unique_ptr<Mesh>>& MeshCache()
{
	static std::unordered_map<std::string, std::unique_ptr<Mesh>> c; return c;
}
static std::unordered_map<std::string, std::unique_ptr<Shader>>& ShaderCache()
{
	static std::unordered_map<std::string, std::unique_ptr<Shader>> c; return c;
}
static std::unordered_map<std::string, unsigned int>& TexCache()
{
	static std::unordered_map<std::string, unsigned int> c; return c;
}
static std::unordered_map<std::string, std::unique_ptr<Material>>& MatCache()
{
	static std::unordered_map<std::string, std::unique_ptr<Material>> c; return c;
}

Mesh* ResourceLib::GetMesh(const char* key)
{
	auto it = MeshCache().find(key);
	if (it != MeshCache().end())
		return it->second.get();

	Mesh* m = nullptr;
	if (strcmp(key, "builtin:cube") == 0)        m = Mesh::CreateCube();
	else if (strcmp(key, "builtin:ground") == 0) m = Mesh::CreateGround();
	else m = Mesh::LoadOBJ((getAssetPath() + key).c_str());
	if (m)
	{
		MeshCache()[key] = std::unique_ptr <Mesh>(m);
		LOG_INFO("Mesh first load: %s", key);
	}
	return m;
}

Shader* ResourceLib::GetShader(const char* vert, const char* frag)
{
	std::string key = std::string(vert) + "|" + frag;
	auto it = ShaderCache().find(key);
	if (it != ShaderCache().end()) return
		it->second.get();

	Shader* s = new Shader(vert, frag);
	if (!s->m_valid)
	{
		delete s;
		return nullptr;
	}
	ShaderCache()[key] = std::unique_ptr<Shader>(s);
	LOG_INFO("Shader first compile: %s", key.c_str());
	return s;
}

unsigned int ResourceLib::GetTexture(const char* path)
{
	auto it = TexCache().find(path);
	if (it != TexCache().end()) return it->second;

	unsigned int t = Material::LoadTexture((getAssetPath() + path).c_str());
	if (t)
	{
		TexCache()[path] = t;
		LOG_INFO("Texture first load: %s", path);
	}
	return t;
}

Material* ResourceLib::GetMaterial(const char* key)
{
	auto it = MatCache().find(key);
	if (it != MatCache().end()) return
		it->second.get();

	Material* m = Material::CreateFromJson(key);
	if (m)
	{
		MatCache()[key] = std::unique_ptr<Material>(m);
		LOG_INFO("Material first load: %s", key);
	}
	return m;
}

void ResourceLib::Shutdown()
{
	MeshCache().clear();
	MatCache().clear();
	ShaderCache().clear();
	for (auto& kv : TexCache()) glDeleteTextures(1, &kv.second);
	TexCache().clear();
	
}






