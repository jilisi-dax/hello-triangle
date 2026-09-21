#include "role.h"
#include "sys.h"
#include "Shader.h"
#include "camera.h"
#include "randerComponent.h"

modelObj::modelObj()
{
	name = "tree";
	SetPos(glm::vec3(2.0f, 0.5f, 0.0f));
	SetScale(glm::vec3(3.0f));
	addComponent(new ModelRenderer());
}

cubeModel::cubeModel()
{
	name = "cube";
	SetPos(glm::vec3(-2.0f, 0.0f, 0.0f));
	SetRotation(glm::vec3(0, 45, 45));
	SetScale(glm::vec3(1.0f, 2.0f, 0.5f));

	addComponent(new CubeRenderer(glm::vec3(1.0f, 1.0f, 1.0f), 8.0f, 0.3f));
	SpinComponent* spin = new SpinComponent();
	spin->axis = glm::vec3(1.0f, -1.0f, 0.0f);
	spin->speed = 45.0f;
	addComponent(spin);
}
metalCube::metalCube()
{
	name = "metalCube";
	SetPos(glm::vec3(0.0f, 0.0f, 2.0f));

	addComponent(new CubeRenderer(glm::vec3(1.0f, 1.0f, 1.0f), 128.0f, 1.5f));
}
ground::ground()
{
	name = "cube";
	addComponent(new GroundRenderer());
}
