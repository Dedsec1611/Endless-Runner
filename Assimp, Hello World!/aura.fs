#version 330 core
out vec4 FragColor;

in vec3 FragPos;

uniform vec3 auraColor;

void main()
{
    float distance = length(FragPos);
    float intensity = 1.0 / (distance * 0.1); // effetto glow attenuato con la distanza
    intensity = clamp(intensity, 0.0, 1.0);

    FragColor = vec4(auraColor, intensity * 0.4); // semitrasparente
}
