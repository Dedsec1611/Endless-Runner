#version 330 core

layout(location = 0) in vec2 aPos;    // posizioni già in NDC [-1..1] che mandi dal CPU
out vec2 ScreenPos;                    // 0..1 per il fragment

void main() {
    ScreenPos   = aPos * 0.5 + 0.5;    // NDC -> [0,1]
    gl_PointSize = 6.0;                // regola più grande/piccolo se vuoi
    gl_Position  = vec4(aPos, 0.0, 1.0);
}