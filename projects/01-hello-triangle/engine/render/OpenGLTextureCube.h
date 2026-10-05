#pragma once
#include "render/TextureCube.h"

class OpenGLTextureCube : public TextureCube
{
public:
	OpenGLTextureCube(const char* const faces[6]);
	~OpenGLTextureCube() override;

	void Bind(int unit) const override;
	bool IsValid() const override { return m_id != 0; }

// 烘焙工厂（抽象类静态成员）要接管现成 id，授权访问私有构造
friend TextureCube* TextureCube::CreateFromEquirect(const char* equirectPath);
friend TextureCube* TextureCube::CreateIrradiance(const TextureCube& env, int faceSize);
friend TextureCube* TextureCube::CreatePrefiltered(const TextureCube& env, int faceSize);

private:
	explicit OpenGLTextureCube(unsigned int prebuiltId) : m_id(prebuiltId) {}
	unsigned int m_id = 0;
};