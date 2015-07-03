/**
    FS_Spectrum.glsl

    @version    0.1
    @author     Miika 'LehdaRi' Lehtimäki
    @date       2015-07-02
**/


#version 400


in vec2 UV;

out vec4 [2] _out;


uniform sampler2D tex1; //  real part of DFT when producing spectra, amp spectrum when producing DFT
uniform sampler2D tex2; //  imag. part of DFT when producing spectra, phase spectrum when producing DFT

uniform bool inverse;


void main() {
    vec2 UV_ = UV + vec2(0.5);
    UV_ -= vec2(int(UV_.x), int(UV_.y));

    _out[0] = texture(tex1, UV_);
    _out[1] = texture(tex2, UV_);

    if (inverse) {
        vec4 temp = _out[0]*cos(_out[1]);
        _out[1] = _out[0]*sin(_out[1]);
        _out[0] = temp;
    }
    else {
        vec4 temp = sqrt(_out[0]*_out[0] + _out[1]*_out[1]);
        _out[1] = atan(_out[1], _out[0]);
        _out[0] = temp;
    }
}
