#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;
in vec3 FragPos;

uniform vec3  viewPos;     // ce l’hai già in C++, lo stai settando
uniform vec3  fogColor;    // es: vec3(0.01, 0.02, 0.05)
uniform float fogDensity;  // es: 0.02..0.04
uniform bool  fogEnabled;  // per abilitare/disabilitare
uniform float fogStart;   // distanza da cui INIZIA il fog (es. 45)
uniform float fogEnd;     // distanza a cui è PIENO fog (es. 120)


uniform sampler2D texture_diffuse1;

void main()
{
    vec3 color = texture(texture_diffuse1, TexCoords).rgb;

   // ---- FOG LINEARE CON START/END ----
// 0 vicino (nessun fog) → 1 lontano (tutto fog)
if (fogEnabled) {
    float dist = length(viewPos - FragPos);
    float fog  = smoothstep(fogStart, fogEnd, dist);
    color = mix(color, fogColor, fog);
}


    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);               // come prima
    FragColor   = vec4(color, 1.0);
}
