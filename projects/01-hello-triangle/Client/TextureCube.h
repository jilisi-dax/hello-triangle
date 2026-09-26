#pragma once
#include "pch.h"

class TextureCube
{
public:
    TextureCube(const char* const faces[6]);
    static TextureCube* CreateFromEquirect(const char* equirectPath);
    ~TextureCube();


    bool IsValid() const { return m_id != 0; }
    unsigned int GetID() const { return m_id; }

private:
    unsigned int m_id = 0;

    explicit TextureCube(unsigned int prebuiltId) : m_id(prebuiltId) {}
};