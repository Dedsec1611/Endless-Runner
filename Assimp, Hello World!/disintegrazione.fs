#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse1;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform float alpha;

void main()
{
    vec3 color = texture(texture_diffuse1, TexCoords).rgb;
    vec3 norm = normalize(Normal);
    vec3 light = normalize(-lightDir);

    float diff = max(dot(norm, light), 0.0);
    vec3 diffuse = diff * lightColor * color;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfDir = normalize(light + viewDir);
    float spec = pow(max(dot(norm, halfDir), 0.0), 32.0);
    vec3 specular = lightColor * spec;

    vec3 finalColor = diffuse + specular;

    FragColor = vec4(finalColor, alpha);
    if (FragColor.a <= 0.01) discard;
}