/**
    FS_FFT.glsl

    @version    0.1
    @author     Miika 'LehdaRi' Lehtimäki
    @date       2015-06-25
**/


#version 400


#define PI 3.14159265359


in vec2 UV;

out vec4 [2] _out;

uniform sampler2D tex_real;
uniform sampler2D tex_img;

uniform uint xDepth, yDepth;    //  the FFT depth
uniform uint direction;         //  transform direction (0: x, 1: y)
uniform uint iter;              //  iteration id

uint    width   = 1 << xDepth;
uint    height  = 1 << yDepth;
uint    px      = uint(UV.x * width);
uint    py      = uint(UV.y * height);
float   xd      = 1.0 / width;
float   yd      = 1.0 / height;

//  brute force DFT for debugging
vec4[2] DFT(uint width, uint k, uint offset, uint stride) {
    vec4 o[2] = vec4[2](vec4(0.0), vec4(0.0));

    for (int n=0; n<width; ++n) {
        vec4 real   = texture(tex_real, vec2((offset + n*stride)*xd, UV.y));
        vec4 img    = texture(tex_img, vec2((offset + n*stride)*xd, UV.y));
        float tReal = cos((2*PI*k*n)/width);
        float tImg  = sin((2*PI*k*n)/width);

        o[0] += real*tReal + img*tImg;
        o[1] += img*tReal - real*tImg;
    }

    return o;
}

vec4[2] FFT(uint n, uint k, uint offset, uint stride) {
    vec4 o[2] = vec4[2](vec4(0.0), vec4(0.0));
    vec4[2] even = vec4[2](vec4(0.0), vec4(0.0));
    vec4[2] odd = vec4[2](vec4(0.0), vec4(0.0));
    if (k < width/2) {
        even    = DFT(width/2, k, 0, 2);
        odd     = DFT(width/2, k, 1, 2);
    }
    else {
        even    = DFT(width/2, k-width/2, 0, 2);
        odd     = DFT(width/2, k-width/2, 1, 2);
    }
    o[0] = even[0] + odd[0]*cos((2*PI*k)/width) + odd[1]*sin((2*PI*k)/width);
    o[1] = even[1] + odd[1]*cos((2*PI*k)/width) - odd[0]*sin((2*PI*k)/width);

    return o;
}

void main() {
    //_out = DFT(width, px, 0, 1);
    //_out = FFT(width, px, 0, 1);
    //return;

    if (iter == 0) {
        //  on iteration 0 just re-arrange the rows / cols
        if (direction == 0) {
            _out[0] = texture(tex_real, vec2((bitfieldReverse(px) >> (32-xDepth))*xd, UV.y));
            _out[1] = vec4(0.0);
        }
    }
    else {
        uint n = 1 << iter;
        uint k = px & ((1 << iter) - 1);
        float a = (2*PI*k)/n;
        float tc = cos(a);
        float ts  = sin(a);

        vec4 eReal  = vec4(0.0);
        vec4 eImg   = vec4(0.0);
        vec4 oReal  = vec4(0.0);
        vec4 oImg   = vec4(0.0);
        if (k < n/2) {
            eReal  = texture(tex_real, vec2(px*xd, UV.y));
            eImg   = texture(tex_img, vec2(px*xd, UV.y));
            oReal  = texture(tex_real, vec2((px+n/2)*xd, UV.y));
            oImg   = texture(tex_img, vec2((px+n/2)*xd, UV.y));
        }
        else {
            eReal  = texture(tex_real, vec2((px-n/2)*xd, UV.y));
            eImg   = texture(tex_img, vec2((px-n/2)*xd, UV.y));
            oReal  = texture(tex_real, vec2(px*xd, UV.y));
            oImg   = texture(tex_img, vec2(px*xd, UV.y));
        }

        _out[0] = eReal + oReal*tc + oImg*ts;
        _out[1] = eImg + oImg*tc - oReal*ts;
    }
}
