#version 330 core
in vec3 worldPos;
out vec4 FragColor;
uniform float cellSize = 2.0f;   // 每个格子边长

void main()
{
    vec2 cell = floor(worldPos.xz / cellSize);
    float checker = mod(cell.x + cell.y, 2.0);
    vec3 color = checker > 0.5 ? vec3(0.85f, 0.82f, 0.78f) : vec3(0.55f, 0.52f, 0.48f);
    FragColor = vec4(color, 1.0);
}
