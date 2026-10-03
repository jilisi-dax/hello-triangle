#pragma once
#include "core/pch.h"
#include "scene/Scene.h"
#include "scene/SceneObject.h"

class modelObj : public SceneObject
{
	public:
		modelObj();
		~modelObj() {};
};

class cubeModel : public SceneObject
{
public:
	cubeModel();
	~cubeModel() {};
};
class metalCube : public SceneObject
{
public:
	metalCube();
	~metalCube() {};
};
class ground : public SceneObject
{
public:

	ground();
	~ground() {
	};
};