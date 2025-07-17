#version 330 core
layout (location = 0) in vec3 aPos;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = vec2(aPos.x * 0.5 + 0.5, aPos.z * 0.5 + 0.5);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
