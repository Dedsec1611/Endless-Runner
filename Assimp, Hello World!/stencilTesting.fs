#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;

uniform vec3 color;
uniform int val;
void main()
{
    
    if(val == 1){
        FragColor = vec4(1.00, 1.00, 0.26, 1.0);
    }
    if(val == 2){
        FragColor = vec4(1.00, 0.00, 0.00, 1.0);
    }
    BrightColor = vec4(FragColor.rgb, 1.0);
}

