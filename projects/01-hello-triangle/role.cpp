#include "role.h"
#include "sys.h"
#include "Shader.h"
#include "camera.h"
#include "randerComponent.h"

modelObj::modelObj()
{
	name = "tree";
	addComponent(new ModelRenderer());
}

cubeModel::cubeModel()
{
	name = "cube";
	addComponent(new CubeRenderer());
}
ground::ground()
{
	name = "cube";
	addComponent(new GroundRenderer());
}

bool loadOBJ(const char* path, std::vector<float>& vertices)
{
	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> normals;
	std::string line;
	std::ifstream file(path);
	if (!file.is_open()) return false;

	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		std::string prefix;
		iss >> prefix;

		if (prefix == "v") {
			glm::vec3 pos;
			iss >> pos.x >> pos.y >> pos.z;
			positions.push_back(pos);
		}
		else if (prefix == "vt") {
			glm::vec2 uv;
			iss >> uv.x >> uv.y;
			texCoords.push_back(glm::vec2(uv.x ,1.0f - uv.y));
		}
		else if (prefix == "vn") {
			glm::vec3 n;
			iss >> n.x >> n.y >> n.z;
			normals.push_back(n);
		}
		else if (prefix == "f") {
			// 每个角格式: vIdx/uvIdx/nIdx，OBJ从1开始，要减1
			for (int i = 0; i < 3; i++) {
				unsigned int vi, ti, ni;
				char slash;
				iss >> vi >> slash >> ti >> slash >> ni;
				vi--; ti--; ni--;
				// 按 位置3 + 法线3 + UV2 = 8个float 交错写入
				vertices.push_back(positions[vi].x);
				vertices.push_back(positions[vi].y);
				vertices.push_back(positions[vi].z);
				vertices.push_back(normals[ni].x);
				vertices.push_back(normals[ni].y);
				vertices.push_back(normals[ni].z);
				vertices.push_back(texCoords[ti].x);
				vertices.push_back(texCoords[ti].y);
			}
		}
	}
	return true;
}

