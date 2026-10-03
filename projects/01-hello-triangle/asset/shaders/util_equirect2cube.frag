#version 330 core
out vec4 FragColor;

in vec3 dir;

uniform sampler2D equirect;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), -asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = SampleSphericalMap(normalize(dir));
    vec3 c = min(texture(equirect, uv).rgb, vec3(1000000.0));
    float m = max(c.r, max(c.g, c.b));
    float T = 90.0, K = 10.0;                 // 肩部起点 + 肩宽，渐近上限 T+K=100
    float x = m - T;
    float s = (x > 0.0) ? (T + K * x / (x + K)) / m : 1.0;
    vec3 color = c * s;
    FragColor = vec4(color, 1.0);
}