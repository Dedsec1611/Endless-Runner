#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform float alpha;

uniform sampler2D backgroundTexture;

void main()
{
FragColor = texture(backgroundTexture, TexCoords) * vec4(1.0, 1.0, 1.0, alpha);}
