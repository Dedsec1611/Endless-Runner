#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

out vec2 TexCoords;

void main()
{
    // Rotazione sull’asse Y
    float angle = time;
    mat4 rotY = mat4(
        cos(angle), 0.0, sin(angle), 0.0,
        0.0,        1.0, 0.0,        0.0,
       -sin(angle), 0.0, cos(angle), 0.0,
        0.0,        0.0, 0.0,        1.0
    );

    TexCoords = aTexCoords;
    gl_Position = projection * view * model * rotY * vec4(aPos, 1.0);
}
