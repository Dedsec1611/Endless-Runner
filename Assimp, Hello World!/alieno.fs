#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse1;

uniform vec3 viewPos;

// ---- FOG ----
uniform vec3  fogColor;    // es: vec3(0.01, 0.02, 0.05)
uniform bool  fogEnabled;  // abilita/disabilita
uniform float fogStart;    // es: 45
uniform float fogEnd;      // es: 120

// ---- SPOTLIGHT (molto debole) ----
struct Spotlight {
    vec3  position;      // posizione del faro
    vec3  direction;     // direzione verso cui punta (dalla luce verso la scena)
    float cutOff;        // cos(radians(angolo interno))
    float outerCutOff;   // cos(radians(angolo esterno))
    // componenti luce (già basse)
    vec3  ambient;       // es: 0.03
    vec3  diffuse;       // es: 0.08
    vec3  specular;      // es: 0.05
    // attenuazione
    float constant;      // es: 1.0
    float linear;        // es: 0.09..0.14 (più alto = più debole)
    float quadratic;     // es: 0.032..0.07
};
uniform Spotlight spotlight;

// ulteriore fattore di “debolezza” globale (per stare sicuri)
uniform float spotStrength; // es: 0.25

// materiale semplice
uniform float shininess; // es: 16.0

void main()
{
    vec3 albedo = texture(texture_diffuse1, TexCoords).rgb;

    // base: solo colore della texture (niente direzionale/point)
    vec3 color = albedo;

    // ---- Spotlight debole ----
    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);
    vec3 L = normalize(spotlight.position - FragPos);

    // bordo morbido
    float theta   = dot(L, normalize(-spotlight.direction)); // direction punta dalla luce verso il target
    float eps     = max(spotlight.cutOff - spotlight.outerCutOff, 0.001);
    float edge    = clamp((theta - spotlight.outerCutOff) / eps, 0.0, 1.0);

    // attenuazione con distanza
    float dist = length(spotlight.position - FragPos);
    float att  = 1.0 / (spotlight.constant + spotlight.linear * dist + spotlight.quadratic * dist * dist);

    // diffusione + speculare (Blinn-Phong)
    float diff = max(dot(N, L), 0.0);
    vec3  H    = normalize(L + V);
    float spec = pow(max(dot(N, H), 0.0), shininess);

    vec3 ambient  = spotlight.ambient  * albedo;
    vec3 diffuse  = spotlight.diffuse  * diff * albedo;
    vec3 specular = spotlight.specular * spec;

    // applica bordo morbido, attenuazione e debolezza globale
    vec3 spotLight = (ambient + diffuse + specular) * edge * att * spotStrength;

    // somma spotlight al colore base (tenue)
    color += spotLight;

    // ---- FOG lineare start/end ----
    if (fogEnabled) {
        float d = length(viewPos - FragPos);
        float f = smoothstep(fogStart, fogEnd, d);
        color = mix(color, fogColor, f);
    }

    FragColor   = vec4(color, 1.0);
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0); // niente bloom
}
