/**
    FS_TextureManip.glsl

    @version    0.1
    @author     Miika 'LehdaRi' Lehtimäki
    @date       2015-06-30
**/


#version 330 core


#define PI 3.14159265359


in vec2 UV;

out vec4 _out[2];

/*
    0: multiply
    1: complexConjMultiply
*/
uniform int func;

uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform sampler2D tex4;

void main() {
    switch (func) {
    case 0:
        //  multiply
        _out[0] = texture(tex1, UV) * texture(tex2, UV);
        _out[1] = vec4(0.0);
        return;

    case 1:
        //  complexConjMultiply
        vec4 lReal = texture(tex1, UV);
        vec4 lImg = texture(tex2, UV);
        vec4 rReal = texture(tex3, UV);
        vec4 rImg = texture(tex4, UV);
        _out[0] = lReal*rReal + lImg*rImg;
        _out[1] = lImg*rReal - lReal*rImg;
        vec4 invNorm = 1.0/sqrt(_out[0]*_out[0] + _out[1]*_out[1]);
        _out[0] *= invNorm;
        _out[1] *= invNorm;
        return;

    default:
        _out[0] = vec4(0.0);
        _out[1] = vec4(0.0);
    }
}

