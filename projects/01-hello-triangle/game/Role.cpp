#include "Role.h"
#include "platform/Paths.h"
#include "render/Shader.h"
#include "scene/components/CameraComponent.h"
#include "scene/components/MeshRendererComponent.h"

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
	SetPos(glm::vec3(-2.0f, -1.0f, 0.0f));
	SetRotation(glm::vec3(0, 45, 45));
	SetScale(glm::vec3(1.0f, 2.0f, 0.5f));

	addComponent(new MeshRenderer("mat/plastic.mat"));
	SpinComponent* spin = new SpinComponent();
	spin->axis = glm::vec3(1.0f, -1.0f, 0.0f);
	spin->speed = 45.0f;
	addComponent(spin);
}
metalCube::metalCube()
{
	name = "metalCube";
	SetPos(glm::vec3(0.0f, -1.5f, 2.0f));

	addComponent(new MeshRenderer("mat/metal.mat"));
}
ground::ground()
{
	name = "ground";
	SetPos(glm::vec3(0.0f, -1.0f, 0.0f));
	addComponent(new GroundRenderer());
}
