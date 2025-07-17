#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_emission1;

void main()
{    
    vec4 diffuse = texture(texture_diffuse1, TexCoords);
    vec4 specular = texture(texture_specular1, TexCoords);
    vec4 emission = texture(texture_emission1, TexCoords);

    
    // Example of combining them, customize as needed:
    vec4 resultColor = diffuse + specular + emission*2.5;
    //vec4 resultColor = diffuse;

    FragColor = resultColor;
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



