/**
    VS_FFT.glsl

    @version    0.1
    @author     Miika 'LehdaRi' Lehtimäki
    @date       2015-06-25
**/


#version 400

layout(location = 0) in vec3 position;

out vec2 UV;

void main() {
    UV = vec2((position.x+1)*0.5, (position.y+1)*0.5);
    gl_Position = vec4(position, 1.0);
}

