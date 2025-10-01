#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN; // se usi normal map

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    // Normal da normal map
    vec3 normal = texture(normalMap, TexCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(TBN * normal);

    // Direzioni
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir  = normalize(viewPos - FragPos);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    vec3 color = texture(diffuseMap, TexCoords).rgb;
    vec3 ambient  = 0.1 * color;
    vec3 diffuse  = diff * color;
    vec3 specular = spec * vec3(0.3);

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
