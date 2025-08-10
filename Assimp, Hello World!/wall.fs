#version 330 core
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

// Camera & base color
uniform vec3 viewPos;
uniform vec3 objectColor;

// Proper GLSL structs for light & material
struct Light { vec3 position; vec3 ambient; vec3 diffuse; vec3 specular; };
uniform Light light;

struct Material { vec3 ambient; vec3 diffuse; vec3 specular; float shininess; };
uniform Material material;

// Space mode (optional, safe if unused)
uniform bool  spaceMode;
uniform float time;
uniform vec3  fenceTint;
uniform float fenceHeight;
uniform float starDensity;

// Fog (optional)
uniform vec3  fogColor;
uniform float fogDensity;
uniform bool  fogEnabled;
uniform float fogStart;
uniform float fogEnd;


// ---------------- helpers for "space" ----------------
float hash21(vec2 p){
    p = fract(p*vec2(123.34, 456.21));
    p += dot(p, p+45.32);
    return fract(p.x*p.y);
}
float noise(vec2 p){
    vec2 i=floor(p), f=fract(p);
    float a=hash21(i);
    float b=hash21(i+vec2(1,0));
    float c=hash21(i+vec2(0,1));
    float d=hash21(i+vec2(1,1));
    vec2 u=f*f*(3.0-2.0*f);
    return mix(mix(a,b,u.x), mix(c,d,u.x), u.y);
}
float fbm(vec2 p){
    float v=0.0, a=0.5;
    for(int i=0;i<4;i++){
        v+=a*noise(p);
        p*=2.0; a*=0.5;
    }
    return v;
}
vec2 getUV(vec3 P, vec3 N){
    float s = (N.x > 0.0) ? 1.0 : -1.0;
    float scaleZ = 0.06;
    float scaleY = 0.35;
    return vec2(P.z * s * scaleZ, P.y * scaleY);
}
vec3 spaceColor(vec3 P, vec3 N, float t){
    vec2 uv = getUV(P, normalize(N));
    uv.x += t * 0.4;
    float neb = fbm(uv*3.0 + vec2(0.0, t*0.05));
    vec3 base = mix(vec3(0.02,0.04,0.08), fenceTint*0.35, neb);
    vec2 cell = floor(uv * 38.0);
    float h   = hash21(cell);
    float star = step(1.0 - clamp(starDensity,0.0,0.04), h);
    vec2 f = fract(uv * 38.0) - 0.5;
    float d = dot(f,f);
    float tw = 0.5 + 0.5*sin(t*8.0 + h*6.2831);
    float glow = exp(-40.0*d) * star * tw;
    float streak = smoothstep(0.985, 1.0, sin(uv.x*20.0 - t*12.0 + uv.y*3.0)) * 0.08;
    float hN = clamp(fenceHeight > 0.0 ? (P.y / fenceHeight) : 0.0, 0.0, 1.0);
    float rim = exp(-24.0 * pow(1.0 - hN, 2.0));
    vec3 col = base + glow * vec3(1.4,1.6,2.2) + streak * fenceTint + rim * fenceTint * 1.3;
    return col;
}

void main(){
    vec3 color;
    if (spaceMode) {
        color = spaceColor(FragPos, Normal, time);
    } else {
        // Phong
        vec3 ambientC  = light.ambient * material.ambient * objectColor;
        vec3 N         = normalize(Normal);
        vec3 L         = normalize(light.position - FragPos);
        float diff     = max(dot(N, L), 0.0);
        vec3 diffuseC  = light.diffuse * (diff * material.diffuse) * objectColor;
        vec3 V         = normalize(viewPos - FragPos);
        vec3 R         = reflect(-L, N);
        float spec     = pow(max(dot(V, R), 0.0), material.shininess);
        vec3 specularC = light.specular * (spec * material.specular);
        color = ambientC + diffuseC + specularC;
    }

    if (fogEnabled) {
    float dist = length(viewPos - FragPos);
    float fog  = smoothstep(fogStart, fogEnd, dist);
    color = mix(color, fogColor, fog);
}


    FragColor = vec4(color, 1.0);
}
