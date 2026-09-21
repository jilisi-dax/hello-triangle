#pragma once
#include "mesh.h"


void Mesh::build(const std::vector<float>& verts)
{
    vertexCount = (int)verts.size() / 8;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

void Mesh::build(const std::vector<float>& verts, const std::vector<unsigned int>& indices)
{
    build(verts);
    useIndex = true;
    vertexCount = (int)indices.size();
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);
}

Mesh* Mesh::CreateCube()
{
    float cubeVertices[192] = {
        // 后(-Z)，法线(0,0,-1)
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f,
        // 前(+Z)，法线(0,0,1)
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f, 1.0f,
        // 左(-X)，法线(-1,0,0)
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f, 0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f, 0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
        // 右(+X)，法线(1,0,0)
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
         // 下(-Y)，法线(0,-1,0)
         -0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
          0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
          0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
         -0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f,
         // 上(+Y)，法线(0,1,0)
         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, 0.0f,
          0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f, 0.0f,
          0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  1.0f, 1.0f,
         -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,
    };
    unsigned int cubeIndices[36] = {
        0,1,2, 0,2,3,      // 后
        4,5,6, 4,6,7,      // 前
        8,9,10, 8,10,11,   // 左
        12,13,14, 12,14,15,// 右
        16,17,18, 16,18,19,// 下
        20,21,22, 20,22,23 // 上
    };
    Mesh* m = new Mesh();
    m->build(std::vector<float>(std::begin(cubeVertices), std::end(cubeVertices)),
        std::vector<unsigned int>(std::begin(cubeIndices), std::end(cubeIndices)));
    return m;
}

Mesh* Mesh::CreateGround()
{
    static const float v[] = {
        -50.0f, -2.0f, -50.0f,   0,1,0,   0,0,
         50.0f, -2.0f, -50.0f,   0,1,0,   0,0,
         50.0f, -2.0f,  50.0f,   0,1,0,   0,0,
         50.0f, -2.0f,  50.0f,   0,1,0,   0,0,
        -50.0f, -2.0f,  50.0f,   0,1,0,   0,0,
        -50.0f, -2.0f, -50.0f,   0,1,0,   0,0,
    };
    Mesh* m = new Mesh();
    m->build(std::vector<float>(std::begin(v), std::end(v)));
    return m;
}

Mesh* Mesh::LoadOBJ(const char* path)
{
    std::vector<float> verts;
    if (!ParseOBJ(path, verts)) return nullptr;   // 失败返回空指针，调用方负责判
    Mesh* m = new Mesh();
    m->build(verts);
    return m;
}

bool Mesh::ParseOBJ(const char* path, std::vector<float>& vertices)
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
            texCoords.push_back(glm::vec2(uv.x, 1.0f - uv.y));
        }
        else if (prefix == "vn") {
            glm::vec3 n;
            iss >> n.x >> n.y >> n.z;
            normals.push_back(n);
        }
        else if (prefix == "f") {
            struct Corner { int vi, ti, ni; };
            std::vector<Corner> corners;
            std::string tok;
            while (iss >> tok) {
                Corner c{ 0, 0, 0 };
                if (sscanf_s(tok.c_str(), "%d/%d/%d", &c.vi, &c.ti, &c.ni) != 3) {
                    LOG_ERROR("ParseOBJ: unsupported face token '%s' in %s", tok.c_str(), path);
                    return false;   // v、v/vt、v//vn 等格式一律拒绝，不静默置 0
                }
                corners.push_back(c);
            }
            if (corners.size() < 3) {
                LOG_ERROR("ParseOBJ: degenerate face in %s", path);
                return false;
            }

            // OBJ 索引从 1 开始；负索引 = 倒数第 n 个，统一转成 0-based
            auto resolve = [](int idx, size_t count) {
                return idx > 0 ? idx - 1 : idx + (int)count;
                };

            // 扇形三角化：n 边形拆成 n-2 个三角形 (v0, vi, vi+1)
            for (size_t i = 1; i + 1 < corners.size(); i++) {
                Corner tri[3] = { corners[0], corners[i], corners[i + 1] };
                for (const Corner& c : tri) {
                    int vi = resolve(c.vi, positions.size());
                    int ti = resolve(c.ti, texCoords.size());
                    int ni = resolve(c.ni, normals.size());
                    if (vi < 0 || vi >= (int)positions.size() ||
                        ti < 0 || ti >= (int)texCoords.size() ||
                        ni < 0 || ni >= (int)normals.size()) {
                        LOG_ERROR("ParseOBJ: index out of range in %s", path);
                        return false;
                    }
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
    }
    return true;
}
