#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1; // caricata da Model
uniform float time;

void main()
{
vec4 texColor = texture(texture_diffuse1, TexCoords);
float glow = abs(sin(time * 2.0));

// amplifica la luminosità: effetto "faro"
vec3 brightColor = texColor.rgb * vec3(2.5, 2.2, 1.0); // più dorato

// clamp per evitare overflow se superiamo 1.0
brightColor = clamp(brightColor, 0.0, 1.0);

// combinazione finale
FragColor = vec4(brightColor * glow, texColor.a);

}
