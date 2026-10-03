#version 330 core
in vec2 UV;
out vec4 FragColor;

uniform sampler2D screenTex;
uniform float threshold = 1.0;
uniform float knee = 1.0;   // 软膝宽度：阈值两侧平滑过渡

void main()
{
    vec3 color = texture(screenTex, UV).rgb;
    float luma = dot(color, vec3(0.2126, 0.7152, 0.0722));
    
    float soft = clamp(luma - threshold + knee, 0.0, 2.0 * knee);
    soft = soft * soft / (4.0 * knee + 1e-4);
    float contribution = max(luma - threshold, soft);
    FragColor = vec4(color * contribution / max(luma, 1e-4), 1.0);
}
