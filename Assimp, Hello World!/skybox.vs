#version 330 core
layout(location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords   = aPos;
    // posizione in clip space, w duplicato per mantenere la depth = 1.0
    vec4 pos    = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}
