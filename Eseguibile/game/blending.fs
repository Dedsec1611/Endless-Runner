#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform float alpha;

void main()
{             
    vec4 texColor = texture(texture_diffuse1, TexCoords); // Otteniamo il colore della texture come vec4
    texColor.a = alpha; // Impostiamo l'alpha

    // Impostiamo BrightColor come nero (potrebbe essere diverso se vuoi effetti di luminosità)
    BrightColor = vec4(0.0, 0.0, 0.0, 0.0);

    // Impostiamo FragColor con il colore della texture e l'alpha modificati
    FragColor = texColor;
}