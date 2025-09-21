#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform float alpha;

void main()
{             
    vec4 texColor = texture(texture_diffuse1, TexCoords); 
    texColor.a = alpha; 

    BrightColor = vec4(0.0, 0.0, 0.0, 0.0);

    FragColor = texColor;
}