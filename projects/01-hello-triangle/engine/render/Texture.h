#pragma once
#include "core/pch.h"

class Texture
{
public:
	virtual ~Texture() = default;
	virtual void Bind(int unit) const = 0;
	virtual bool IsValid() const = 0;
};