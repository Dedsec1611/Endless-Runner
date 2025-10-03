#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform float time;
uniform vec3 auraColor = vec3(1.0, 0.95, 0.2);   // giallo “bonus”
uniform float intensity = 1.6;                   // intensità base

void main()
{
    float pulse = 0.6 + 0.4 * sin(time * 8.0);
    vec3 col    = auraColor * intensity * pulse;

    // semi-trasparente per miscelarsi bene
    FragColor   = vec4(col, 0.85 * pulse);

    // canale bloom bello forte
    BrightColor = vec4(col, 1.0);
}
