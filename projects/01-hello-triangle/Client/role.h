#pragma once
#include "pch.h"
#include "Scene.h"

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