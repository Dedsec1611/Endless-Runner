#version 330 core
in  vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D particellaTexture;

void main()
{
    // campiona la texture di esplosione
    vec4 col = texture(particellaTexture, TexCoords);
    // scarta i frammenti trasparenti
    if (col.a < 0.1)
        discard;
    FragColor = col;
}
