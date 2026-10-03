#pragma once
#include "render/Mesh.h"


void Mesh::build(const std::vector<float>& verts)
{
    unsigned int stepSize = 11 * sizeof(float);
    vertexCount = (int)verts.size() / 11;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stepSize, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stepSize, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stepSize, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stepSize, (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
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
        0,2,1, 0,3,2,      // 后
        4,5,6, 4,6,7,      // 前
        8,9,10, 8,10,11,   // 左
        12,14,13, 12,15,14,// 右
        16,17,18, 16,18,19,// 下
        20,22,21, 20,23,22 // 上
    };
    static const float faceT[6][3] = {
        {1,0,0}, {1,0,0},   // 后 前
        {0,0,1}, {0,0,1},   // 左 右
        {1,0,0}, {1,0,0},   // 下 上
    };
    std::vector<float> verts;
    verts.reserve(24 * 11);
    for (int face = 0; face < 6; face++)
        for (int k = 0; k < 4; k++)
        {
            const float* src = cubeVertices + (face * 4 + k) * 8;
            verts.insert(verts.end(), src, src + 8);
            verts.insert(verts.end(), faceT[face], faceT[face] + 3);
        }
    std::vector<unsigned int> indices(std::begin(cubeIndices), std::end(cubeIndices));
    Mesh* m = new Mesh();
    m->build(verts, indices);
    return m;
}

Mesh* Mesh::CreateGround()
{
    static const float v[] = {
        -50.0f, -2.0f, -50.0f,   0,1,0,   0,0,
         50.0f, -2.0f,  50.0f,   0,1,0,   0,0,
         50.0f, -2.0f, -50.0f,   0,1,0,   0,0,
         50.0f, -2.0f,  50.0f,   0,1,0,   0,0,
        -50.0f, -2.0f, -50.0f,   0,1,0,   0,0,
        -50.0f, -2.0f,  50.0f,   0,1,0,   0,0,
    };
    const float TILE = 4.0f;
    std::vector<float> verts;
    for (int i = 0; i < 6; i++)
    {
        const float* src = v + i * 8;
        verts.insert(verts.end(), src, src + 6);
        verts.push_back(src[0] / TILE);
        verts.push_back(src[2] / TILE);
        verts.push_back(1.0f); verts.push_back(0.0f); verts.push_back(0.0f);
    }
    Mesh* m = new Mesh();
    m->build(verts);
    return m;
}

Mesh* Mesh::CreateSphere(int stacks, int slices)
{
    std::vector<float> verts;
    std::vector<unsigned int> indices;
    const float R = 0.5f;
    for (int i = 0; i <= stacks; i++)
    {
        float v = (float)i / stacks;
        float phi = v * glm::pi<float>();

        for (int j = 0; j <= slices; j++)
        {
            float u = (float)j / slices;
            float theta = u * 2.0f * glm::pi<float>();
            float x = sin(phi) * cos(theta);
            float y = cos(phi);
            float z = sin(phi) * sin(theta);

            float len = sqrt(x * x + z * z);        //切线
            float tx = 1.0f, tz = 0.0f;
            if (len > 1e-5f) { tx = -z / len; tz = x / len; }

            verts.push_back(x * R); verts.push_back(y * R); verts.push_back(z * R);
            verts.push_back(x);     verts.push_back(y);     verts.push_back(z);
            verts.push_back(u);     verts.push_back(v);
            verts.push_back(tx);    verts.push_back(0.0f); verts.push_back(tz);
        }
    }
    for (int i = 0; i < stacks; i++)
    {
        for (int j = 0; j < slices; j++)
        {
            unsigned int a = i * (slices + 1) + j;   // 格子四角：a b / c d
            unsigned int b = a + 1;
            unsigned int c = a + slices + 1;
            unsigned int d = c + 1;
            indices.push_back(a); indices.push_back(b); indices.push_back(c);
            indices.push_back(b); indices.push_back(d); indices.push_back(c);
        }
    }
    Mesh* m = new Mesh();
    m->build(verts, indices);
    return m;
}

Mesh* Mesh::LoadOBJ(const char* path)
{
    std::vector<float> verts;
    if (!ParseOBJ(path, verts)) return nullptr;
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

                // 取出三角形的三个角（位置 + UV），索引解析与范围检查同上
                glm::vec3 P[3]; glm::vec2 UV[3];
                for (int k = 0; k < 3; k++) {
                    int vi = resolve(tri[k].vi, positions.size());
                    int ti = resolve(tri[k].ti, texCoords.size());
                    int ni = resolve(tri[k].ni, normals.size());
                    if (vi < 0 || vi >= (int)positions.size() ||
                        ti < 0 || ti >= (int)texCoords.size() ||
                        ni < 0 || ni >= (int)normals.size()) {
                        LOG_ERROR("ParseOBJ: index out of range in %s", path);
                        return false;
                    }
                    P[k] = positions[vi];
                    UV[k] = texCoords[ti];
                    tri[k].ni = ni;   // 后面写法线还要用
                }

                // Lengyel 公式：UV 差分求切线（纹理 U 在模型空间的方向）
                glm::vec3 e1 = P[1] - P[0], e2 = P[2] - P[0];
                glm::vec2 duv1 = UV[1] - UV[0], duv2 = UV[2] - UV[0];
                float det = duv1.x * duv2.y - duv1.y * duv2.x;
                glm::vec3 T(1.0f, 0.0f, 0.0f);          // UV 退化（全是同一个点）时的占位
                if (fabs(det) > 1e-8f)
                    T = glm::normalize((e1 * duv2.y - e2 * duv1.y) / det);

                for (int k = 0; k < 3; k++) {
                    const Corner& c = tri[k];
                    vertices.push_back(P[k].x); vertices.push_back(P[k].y); vertices.push_back(P[k].z);
                    vertices.push_back(normals[c.ni].x); vertices.push_back(normals[c.ni].y); vertices.push_back(normals[c.ni].z);
                    vertices.push_back(UV[k].x); vertices.push_back(UV[k].y);
                    vertices.push_back(T.x); vertices.push_back(T.y); vertices.push_back(T.z);
                }
            }
        }
    }
    return true;
}
