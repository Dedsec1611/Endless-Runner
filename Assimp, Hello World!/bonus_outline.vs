#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// parametri aura
uniform float time;
uniform float outlineWidth = 0.05;  // spessore base

void main()
{
    // estrusione lungo la normale in SPAZIO MONDO (niente scale strane)
    vec3 worldPos = vec3(model * vec4(aPos, 1.0));
    vec3 worldN   = normalize(mat3(transpose(inverse(model))) * aNormal);

    float pulse = outlineWidth * (1.0 + 0.25 * sin(time * 8.0));
    worldPos += worldN * pulse;

    gl_Position = projection * view * vec4(worldPos, 1.0);
}
