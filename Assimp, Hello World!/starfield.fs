#version 330 core

in vec2 ScreenPos;                     // NIENTE layout(location=...) qui!
out vec4 FragColor;

uniform float time;
uniform float warp;                    // intensità “scorrimento”
uniform vec2  screenCenter;            // di solito (0.5, 0.5)

void main() {
    // Effetto semplice ma carino: leggero bagliore radiale verso il centro
    vec2  d  = ScreenPos - screenCenter;         // vettore dal centro
    float r  = length(d);
    float twinkle = 0.65 + 0.35 * sin(60.0 * (d.x + d.y) + time * 9.0);
    float streak  = exp(-10.0 * r) * (1.0 + warp * 0.08);  // scia verso il centro

    vec3 color = vec3(0.85, 0.9, 1.0) * twinkle * streak;
    FragColor  = vec4(color, 1.0);
}