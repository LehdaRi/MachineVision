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

void main() {
    _out[0] = vec4(0.0, 0.0, 0.0, 1.0);
    for (int i=0; i<width; ++i) {
        vec4 x = texture(tex_real, vec2(i*xd, UV.y));
        _out[0] += x*cos((2*PI*px*i)/width);
    }
    _out[1] = vec4(0.0, 0.0, 0.0, 1.0);
    return;

    if (iter == 0) {
        //  on iteration 0 just re-arrange the rows / cols
        if (direction == 0) {
            _out[0] = texture(tex_real, vec2((bitfieldReverse(px) >> (32-xDepth))*xd, UV.y));
            _out[1] = vec4(0.0);
        }
    }
    else {
        //  first find out if the row / col is even / odd
        uint n = 1 << iter;  // size of the fft
        uint k = px & ((1 << iter) - 1);

        if (k<n/2) {
            vec4 eReal  = texture(tex_real, vec2(px*xd, UV.y));
            vec4 eImg   = texture(tex_img, vec2(px*xd, UV.y));
            vec4 oReal  = texture(tex_real, vec2((px+n/2)*xd, UV.y));
            vec4 oImg   = texture(tex_img, vec2((px+n/2)*xd, UV.y));
            float tReal = cos((2*PI*k)/n);
            float tImg  = -sin((2*PI*k)/n);

            _out[0] = eReal + oReal*tReal - oImg*tImg;
            _out[1] = eImg + oImg*tReal + oReal*tImg;

            /*_out[0] = (eReal + oReal)/2;
            _out[1] = (eReal + oReal)/2;
            if (iter == 8) {
                _out[0] = vec4((eReal.rg + oReal.rg)/2, float(k)/width, 1.0);
                _out[1] = vec4((eReal.rg + oReal.rg)/2, float(k)/width, 1.0);
            }*/
        }
        else {
            vec4 eReal  = texture(tex_real, vec2((px-n/2)*xd, UV.y));
            vec4 eImg   = texture(tex_img, vec2((px-n/2)*xd, UV.y));
            vec4 oReal  = texture(tex_real, vec2(px*xd, UV.y));
            vec4 oImg   = texture(tex_img, vec2(px*xd, UV.y));
            float tReal = cos((2*PI*k)/n);
            float tImg  = -sin((2*PI*k)/n);

            _out[0] = eReal - oReal*tReal + oImg*tImg;
            _out[1] = eImg - oImg*tReal - oReal*tImg;

            /*_out[0] = (eReal + oReal)/2;
            _out[1] = (eReal + oReal)/2;
            if (iter == 8) {
                _out[0] = vec4((eReal.rg + oReal.rg)/2, float(k)/width, 1.0);
                _out[1] = vec4((eReal.rg + oReal.rg)/2, float(k)/width, 1.0);
            }*/
        }

        //_out = texture(tex, UV);
    }
}

