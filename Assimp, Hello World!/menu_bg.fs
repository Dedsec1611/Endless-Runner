#version 330 core
in vec2 vUV;
out vec4 FragColor;

uniform float time;
uniform vec2  iResolution;

float hash12(vec2 p){ 
    vec3 p3 = fract(vec3(p.xyx) * 0.1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}

void main(){
    vec2 uv = vUV;              
    vec2 p = (uv - 0.5) * vec2(iResolution.x / iResolution.y, 1.0);

    float r = length(p);
    vec3 base = mix(vec3(0.02,0.03,0.08), vec3(0.00,0.02,0.05), smoothstep(0.0,1.2,r));

    float ang = atan(p.y, p.x);
    float neb = 0.25 + 0.25 * sin(5.0*ang + 0.7*time) * exp(-2.5*r);
    base += vec3(0.05, 0.09, 0.18) * neb;

    float stars = step(0.997, hash12(uv*vec2(800.0, 450.0) + floor(time*30.0)));
    base += vec3(0.8, 0.9, 1.0) * stars * 0.08;

    float vig = smoothstep(1.2, 0.2, r);
    base *= mix(0.6, 1.0, vig);

    float scan = 0.03 * sin((uv.y + time*0.15) * iResolution.y * 3.14159);
    base += vec3(scan);

    FragColor = vec4(base, 1.0);
}
