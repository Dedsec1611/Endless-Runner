#version 330 core
in vec3 fragColor;
out vec4 FragColor;

void main() {
    float dist = length(gl_PointCoord - vec2(0.5));
    float alpha = smoothstep(0.5, 0.0, dist); // glow effect
    FragColor = vec4(fragColor, alpha);
}
