#version 330 core

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

layout (location = 0) out vec4 FragColor;      // colore scena
layout (location = 1) out vec4 BrightColor;    // pass luminoso per bloom

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

uniform sampler2D texture_diffuse1;   // se il modello ha una diffuse
uniform Material material;
uniform Light light;
uniform vec3 viewPos;

// parametri “bonus”
uniform vec3 auraColor = vec3(1.0, 0.95, 0.2);
uniform float emissiveStrength = 2.5;      // quanto “brilla”
uniform float time;

void main()
{
    // base PBR/Phong semplice
    vec3 albedo   = texture(texture_diffuse1, TexCoords).rgb;
    vec3 ambient  = light.ambient  * material.ambient;
    vec3 N        = normalize(Normal);
    vec3 L        = normalize(light.position - FragPos);
    float diff    = max(dot(N, L), 0.0);
    vec3 diffuse  = light.diffuse * (diff * material.diffuse);

    vec3 V        = normalize(viewPos - FragPos);
    vec3 R        = reflect(-L, N);
    float spec    = pow(max(dot(V, R), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    vec3 lit = (ambient + diffuse + specular) * albedo;

    // EMISSIONE “bonus”: leggero rim + pulsazione
    float rim     = pow(1.0 - max(dot(N, V), 0.0), 2.0);
    float pulse   = 0.65 + 0.35 * sin(time * 8.0);
    vec3 emissive = auraColor * emissiveStrength * rim * pulse;

    vec3 finalCol = lit + emissive;
    FragColor     = vec4(finalCol, 1.0);

    // canale bright per il bloom: mandiamo SOLO l’emissione (molto intenso)
    BrightColor   = vec4(emissive, 1.0);
}
