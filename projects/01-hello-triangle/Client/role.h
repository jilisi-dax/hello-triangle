#pragma once
#include "pch.h"
#include "camera.h"
#include "Shader.h"
#include "Scene.h"
using namespace std;

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
class ground : public SceneObject
{
public:

	ground();
	~ground() {
	};
};