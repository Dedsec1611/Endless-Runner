#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform vec3 offset;
uniform mat4 view;
uniform mat4 projection;

void main() {
    TexCoords = aTexCoords;
    vec4 worldPos = vec4(offset + vec3(aPos, 0.0), 1.0);
    gl_Position = projection * view * worldPos;
}
