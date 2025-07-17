uniform sampler2D uBackgroundTexture;

in vec2 TexCoords; 
out vec4 FragColor;

void main() {
    FragColor = texture(uBackgroundTexture, TexCoords);
}
