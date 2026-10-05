#include "render/ResourceLib.h"
#include "platform/Paths.h"
#include <cstring>
#include "render/TextureCube.h"
#include "render/Texture2D.h"

static std::unordered_map<std::string, std::unique_ptr<Mesh>>& MeshCache()
{
	static std::unordered_map<std::string, std::unique_ptr<Mesh>> c; return c;
}
static std::unordered_map<std::string, std::unique_ptr<Shader>>& ShaderCache()
{
	static std::unordered_map<std::string, std::unique_ptr<Shader>> c; return c;
}
static std::unordered_map<std::string, Texture2D*>& TexCache()
{
	static std::unordered_map<std::string, Texture2D*> c; return c;
}
static std::unordered_map<std::string, std::unique_ptr<Material>>& MatCache()
{
	static std::unordered_map<std::string, std::unique_ptr<Material>> c; return c;
}
static std::unordered_map<std::string, std::unique_ptr<TextureCube>>& SkyboxCache()
{
	static std::unordered_map<std::string, std::unique_ptr<TextureCube>> c; return c;
}
static std::unordered_map<std::string, std::unique_ptr<TextureCube>>& IrradianceCache()
{
	static std::unordered_map<std::string, std::unique_ptr<TextureCube>> c; return c;
}
static std::unordered_map<std::string, std::unique_ptr<TextureCube>>& PrefilteredCache()
{
	static std::unordered_map<std::string, std::unique_ptr<TextureCube>> c; return c;
}

Mesh* ResourceLib::GetMesh(const char* key)
{
	auto it = MeshCache().find(key);
	if (it != MeshCache().end())
		return it->second.get();

	Mesh* m = nullptr;
	if (strcmp(key, "builtin:cube") == 0)        m = Mesh::CreateCube();
	else if (strcmp(key, "builtin:ground") == 0) m = Mesh::CreateGround();
	else if (strcmp(key, "builtin:sphere") == 0) m = Mesh::CreateSphere();
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

	Shader* s = Shader::Create(vert, frag);
	if (!s->IsValid())
	{
		delete s;
		return nullptr;
	}
	ShaderCache()[key] = std::unique_ptr<Shader>(s);
	LOG_INFO("Shader first compile: %s", key.c_str());
	return s;
}

Texture2D* ResourceLib::GetTexture(const char* path)
{
	auto it = TexCache().find(path);
	if (it != TexCache().end()) return it->second;

	Texture2D* t = Texture2D::Create((getAssetPath() + path).c_str());   // 缓存 key 用原始相对路径，加载补资产根
	if (t)
	{
		TexCache()[path] = t;
		LOG_INFO("Texture first load: %s", path);
	}
	return t;
}

TextureCube* ResourceLib::GetSkybox(const char* dir)
{
	auto it = SkyboxCache().find(dir);
	if (it != SkyboxCache().end()) return it->second.get();

	std::string path(dir);
	TextureCube* t = nullptr;
	if (path.size() > 4 && path.compare(path.size() - 4, 4, ".hdr") == 0)
	{
		t = TextureCube::CreateFromEquirect(dir);
	}
	else
	{
		static const char* faceNames[6] = {
			"right.jpg", "left.jpg", "top.jpg",
			"bottom.jpg", "front.jpg", "back.jpg" };
		std::string paths[6];
		for (int i = 0; i < 6; i++)
			paths[i] = std::string(dir) + "/" + faceNames[i];
		const char* fp[6] = {
			paths[0].c_str(), paths[1].c_str(), paths[2].c_str(),
			paths[3].c_str(), paths[4].c_str(), paths[5].c_str() };
		t = TextureCube::Create(fp);
	}
	if (!t || !t->IsValid()) { delete t; return nullptr; }
	SkyboxCache()[dir] = std::unique_ptr<TextureCube>(t);
	LOG_INFO("Skybox first load: %s", dir);
	return t;
}

TextureCube* ResourceLib::GetIrradiance(const char* dir)
{
	auto it = IrradianceCache().find(dir);
	if (it != IrradianceCache().end()) return it->second.get();

	TextureCube* sky = GetSkybox(dir);
	if (!sky || !sky->IsValid()) return nullptr;

	TextureCube* t = TextureCube::CreateIrradiance(*sky);
	if (!t) return nullptr;
	IrradianceCache()[dir] = std::unique_ptr<TextureCube>(t);
	LOG_INFO("Irradiance first bake: %s", dir);
	return t;
}

TextureCube* ResourceLib::GetPrefiltered(const char* dir)
{
	auto it = PrefilteredCache().find(dir);
	if (it != PrefilteredCache().end()) return it->second.get();

	TextureCube* sky = GetSkybox(dir);
	if (!sky || !sky->IsValid()) return nullptr;

	TextureCube* t = TextureCube::CreatePrefiltered(*sky);
	if (!t) return nullptr;
	PrefilteredCache()[dir] = std::unique_ptr<TextureCube>(t);
	LOG_INFO("Prefiltered first bake: %s", dir);
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
	SkyboxCache().clear();
	IrradianceCache().clear();
	PrefilteredCache().clear();
	TexCache().clear();   // unique_ptr 自动 delete 资源对象，析构里对称删 GL 纹理
	
}







Texture2D* ResourceLib::GetBrdfLut()
{
	static Texture2D* s_lut = nullptr;
	if (!s_lut)
		s_lut = Texture2D::CreateBrdfLut();
	return s_lut;
}
