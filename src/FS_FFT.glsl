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
uniform bool inverse;
uniform bool spectrum;

uint    width   = 1 << xDepth;
uint    height  = 1 << yDepth;
uint    px      = uint(UV.x * width);
uint    py      = uint(UV.y * height);
float   xd      = 1.0 / width;
float   yd      = 1.0 / height;


void main() {
    vec4 eReal  = vec4(0.0);
    vec4 eImg   = vec4(0.0);
    vec4 oReal  = vec4(0.0);
    vec4 oImg   = vec4(0.0);
    float tc, ts;

    if (direction == 0) {
        if (iter == 0) {
            //  on iteration 0 just re-arrange the rows / cols
            _out[0] = texture(tex_real, vec2((bitfieldReverse(px) >> (32-xDepth))*xd, UV.y));
            _out[1] = texture(tex_img, vec2((bitfieldReverse(px) >> (32-xDepth))*xd, UV.y));
        }
        else {
            uint n = 1 << iter;
            uint k = px & ((1 << iter) - 1);
            float a = (2*PI*k)/n;
            if (inverse)
                a *= -1;
            tc = cos(a);
            ts = sin(a);

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

            if (iter == xDepth && inverse) {
                _out[0] /= width;
                _out[1] /= width;
            }
        }
    }
    else { //   Y-direction
        if (iter == 0) {
            _out[0] = texture(tex_real, vec2(UV.x, (bitfieldReverse(py) >> (32-yDepth))*yd));
            _out[1] = texture(tex_img, vec2(UV.x, (bitfieldReverse(py) >> (32-yDepth))*yd));
        }
        else {
            uint n = 1 << iter;
            uint k = py & ((1 << iter) - 1);
            float a = (2*PI*k)/n;
            if (inverse)
                a *= -1;
            tc = cos(a);
            ts = sin(a);

            if (k < n/2) {
                eReal  = texture(tex_real, vec2(UV.x, py*yd));
                eImg   = texture(tex_img, vec2(UV.x, py*yd));
                oReal  = texture(tex_real, vec2(UV.x, (py+n/2)*yd));
                oImg  = texture(tex_img, vec2(UV.x, (py+n/2)*yd));
            }
            else {
                eReal  = texture(tex_real, vec2(UV.x, (py-n/2)*yd));
                eImg   = texture(tex_img, vec2(UV.x, (py-n/2)*yd));
                oReal  = texture(tex_real, vec2(UV.x, py*yd));
                oImg   = texture(tex_img, vec2(UV.x, py*yd));
            }

            _out[0] = eReal + oReal*tc + oImg*ts;
            _out[1] = eImg + oImg*tc - oReal*ts;

            if (iter == yDepth && inverse) {
                _out[0] /= height;
                _out[1] /= height;
            }

            if (iter == yDepth && spectrum && !inverse) {
                if (inverse) {
                    _out[0] /= height;
                    _out[1] /= height;
                }
                else if (spectrum) {
                    vec4 temp = sqrt(_out[0]*_out[0] + _out[1]*_out[1]) / height;
                    _out[1] = atan(_out[1], _out[0]);
                    _out[0] = temp;
                }
            }
        }
    }

    //_out[0] = eReal + oReal*tc + oImg*ts;
    //_out[1] = eImg + oImg*tc - oReal*ts;
}
