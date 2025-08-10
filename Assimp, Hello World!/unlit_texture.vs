#version 330 core
in  vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D particleTexture;

void main()
{
    vec4 col = texture(particleTexture, TexCoords);
    if (col.a < 0.1) discard;
    FragColor = col;
}
