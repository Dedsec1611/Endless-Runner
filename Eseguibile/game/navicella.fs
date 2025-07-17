#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_emission1;

void main()
{
    vec3 color = texture(texture_diffuse1, TexCoords).rgb;
    vec3 emission = texture(texture_emission1, TexCoords).rgb*10;

    vec3 finalColor = color + emission;

    FragColor = vec4(finalColor, 1.0);
    
    // Consider emission when determining brightness for BrightColor
    float brightness = dot(finalColor, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)  // Adjust threshold as necessary
        BrightColor = vec4(finalColor, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}





//#version 330 core
//out vec4 FragColor;

//float near = 0.1; 
//float far  = 100.0; 
  
//float LinearizeDepth(float depth) 
//{
//  float z = depth * 2.0 - 1.0; // back to NDC 
//  return (2.0 * near * far) / (far + near - z * (far - near));
//}

//void main()
//{             
//  float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
//  FragColor = vec4(vec3(depth), 1.0);
//}



