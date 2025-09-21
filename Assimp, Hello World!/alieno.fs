#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse1;

uniform vec3 viewPos;

uniform vec3  fogColor;    
uniform bool  fogEnabled; 
uniform float fogStart;    
uniform float fogEnd;      

struct Spotlight {
    vec3  position;      
    vec3  direction;     
    float cutOff;        
    float outerCutOff;   
    vec3  ambient;       
    vec3  diffuse;       
    vec3  specular;      
    float constant;      
    float linear;        
    float quadratic;     
};
uniform Spotlight spotlight;

uniform float spotStrength; 

uniform float shininess; 

void main()
{
    vec3 albedo = texture(texture_diffuse1, TexCoords).rgb;

    vec3 color = albedo;

    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);
    vec3 L = normalize(spotlight.position - FragPos);

    float theta   = dot(L, normalize(-spotlight.direction)); 
    float eps     = max(spotlight.cutOff - spotlight.outerCutOff, 0.001);
    float edge    = clamp((theta - spotlight.outerCutOff) / eps, 0.0, 1.0);

    float dist = length(spotlight.position - FragPos);
    float att  = 1.0 / (spotlight.constant + spotlight.linear * dist + spotlight.quadratic * dist * dist);

    float diff = max(dot(N, L), 0.0);
    vec3  H    = normalize(L + V);
    float spec = pow(max(dot(N, H), 0.0), shininess);

    vec3 ambient  = spotlight.ambient  * albedo;
    vec3 diffuse  = spotlight.diffuse  * diff * albedo;
    vec3 specular = spotlight.specular * spec;

    vec3 spotLight = (ambient + diffuse + specular) * edge * att * spotStrength;

    color += spotLight;

    if (fogEnabled) {
        float d = length(viewPos - FragPos);
        float f = smoothstep(fogStart, fogEnd, d);
        color = mix(color, fogColor, f);
    }

    FragColor   = vec4(color, 1.0);
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0); 
}
