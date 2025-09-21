#version 330 core

in vec2 ScreenPos;                     
out vec4 FragColor;

uniform float time;
uniform float warp;                    
uniform vec2  screenCenter;            

void main() {
    vec2  d  = ScreenPos - screenCenter;        
    float r  = length(d);
    float twinkle = 0.65 + 0.35 * sin(60.0 * (d.x + d.y) + time * 9.0);
    float streak  = exp(-10.0 * r) * (1.0 + warp * 0.08);  

    vec3 color = vec3(0.85, 0.9, 1.0) * twinkle * streak;
    FragColor  = vec4(color, 1.0);
}