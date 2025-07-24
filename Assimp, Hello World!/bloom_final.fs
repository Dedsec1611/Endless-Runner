#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D scene, bloomBlur;
uniform float exposure, saturation, contrast, brightness;
const float gamma = 2.2;

void main(){
    vec3 hdr = texture(scene, TexCoords).rgb;
    hdr += texture(bloomBlur, TexCoords).rgb;

    vec3 gray = vec3(dot(hdr, vec3(0.2126,0.7152,0.0722)));
    vec3 col = mix(gray, hdr, saturation);

    col = (col - 0.5) * contrast + 0.5;
    col *= brightness;

    vec3 mapped = vec3(1.0) - exp(-col * exposure);
    mapped = pow(mapped, vec3(1.0 / gamma));
    FragColor = vec4(mapped, 1.0);
}
