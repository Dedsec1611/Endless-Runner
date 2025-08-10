#version 330 core
out vec4 FragColor;
uniform vec3 color;      // PRIMO CAMBIO: uniform chiama 'color', non objectColor

void main()
{
    FragColor = vec4(color, 1.0);
}
