#version 330 core
out vec4 FragColor;

uniform vec3 color;
uniform float time;

void main()
{
    // Bagliore pulsante tra 0.7 e 1.0
    float pulsazione = 0.7 + 0.3 * sin(time * 10.0);

    // Dissolvenza automatica (oscilla da opaco a trasparente)
    float alpha = 1.0 - mod(time * 0.5, 1.0);

    FragColor = vec4(color * pulsazione, alpha);
}
