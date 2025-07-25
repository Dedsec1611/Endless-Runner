#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform vec4 color;
uniform sampler2D particleTexture;

void main() {
    vec4 texColor = texture(particleTexture, TexCoords);
    FragColor = color * texColor;
}
