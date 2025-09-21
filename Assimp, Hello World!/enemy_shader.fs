#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform vec3  viewPos;

// Fog
uniform vec3  fogColor;
uniform float fogDensity;  
uniform bool  fogEnabled;
uniform float fogStart;
uniform float fogEnd;

// Textures
uniform sampler2D texture_diffuse1;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct Spotlight {
    vec3 position;
    vec3 direction;
    float cutOff;       
    float outerCutOff;  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

uniform Material material;
uniform Light light;
uniform Spotlight spotlight;
uniform bool useSpotlight;

vec3 calcPointLight(vec3 norm, vec3 viewDir, vec3 baseColor)
{
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfDir), 0.0), material.shininess);

    vec3 ambient  = light.ambient  * material.ambient  * baseColor;
    vec3 diffuse  = light.diffuse  * material.diffuse  * diff * baseColor;
    vec3 specular = light.specular * material.specular * spec;

    return ambient + diffuse + specular;
}

vec3 calcSpotlight(vec3 norm, vec3 viewDir, vec3 baseColor)
{
    vec3 lightDir = normalize(spotlight.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    float theta = dot(lightDir, normalize(-spotlight.direction)); 
    float epsilon = clamp(spotlight.cutOff - spotlight.outerCutOff, 0.0001, 1.0);
    float intensity = clamp((theta - spotlight.outerCutOff) / epsilon, 0.0, 1.0);

    float distance = length(spotlight.position - FragPos);
    float attenuation = 1.0 / (spotlight.constant + spotlight.linear * distance +
                               spotlight.quadratic * (distance * distance));

    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfDir), 0.0), material.shininess);

    vec3 ambient  = spotlight.ambient  * material.ambient  * baseColor;
    vec3 diffuse  = spotlight.diffuse  * material.diffuse  * diff * baseColor;
    vec3 specular = spotlight.specular * material.specular * spec;

    vec3 color = ambient + (diffuse + specular) * intensity *0.7;
    color *= attenuation;
    return color;
}

void main()
{
    vec3 albedo = texture(texture_diffuse1, TexCoords).rgb;

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 lighting = calcPointLight(norm, viewDir, albedo);

    if (useSpotlight) {
        lighting += calcSpotlight(norm, viewDir, albedo);
    }

    if (fogEnabled) {
        float dist = length(viewPos - FragPos);
        float f = smoothstep(fogStart, fogEnd, dist);
        lighting = mix(lighting, fogColor, f);
    }

    FragColor = vec4(lighting, 1.0);

    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
