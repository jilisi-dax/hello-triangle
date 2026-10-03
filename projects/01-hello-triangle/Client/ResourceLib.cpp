#include "ResourceLib.h"
#include "sys.h"
#include <cstring>
#include "TextureCube.h"

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
		t = new TextureCube(fp);
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

	TextureCube* t = TextureCube::CreateIrradiance(sky->GetID());
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

	TextureCube* t = TextureCube::CreatePrefiltered(sky->GetID());
	if (!t) return nullptr;
	PrefilteredCache()[dir] = std::unique_ptr<TextureCube>(t);
	LOG_INFO("Prefiltered first bake: %s", dir);
	return t;
}

unsigned int ResourceLib::GetBrdfLut()
{
	static unsigned int s_lut = 0;
	if (s_lut) return s_lut;
	Shader* s = ResourceLib::GetShader(
		(getAssetPath() + "shaders/post.vert").c_str(),
		(getAssetPath() + "shaders/util_brdf_lut.frag").c_str()
	);
	if (!s) return 0;

	float quad[] = {
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,
	};
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glBindVertexArray(0);

	unsigned int tex, fbo;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
	glViewport(0, 0, 512, 512);
	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT);

	s->UseProgram();
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	glDeleteFramebuffers(1, &fbo);

	LOG_INFO("BrdfLut baked: 512x512");
	s_lut = tex;
	return s_lut;
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
	for (auto& kv : TexCache()) glDeleteTextures(1, &kv.second);
	TexCache().clear();
	
}






