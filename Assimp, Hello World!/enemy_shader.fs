#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

uniform vec3 lightDir = normalize(vec3(-0.5, -1.0, -0.3));
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);

void main()
{
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, -lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 textureColor = texture(texture_diffuse1, TexCoords).rgb;
    vec3 result = diffuse * textureColor;

    FragColor = vec4(result, 1.0);
}
