/**
    FS_Texture.glsl

    @version    0.1
    @author     Miika 'LehdaRi' Lehtimäki
    @date       2015-04-18
**/


#version 330 core

in vec2 UV;

out vec4 color;

uniform sampler2D tex1;
uniform sampler2D tex2;

void main() {
    if (UV.x < 0.5)
        color = vec4((texture(tex1, vec2(UV.x*2, UV.y)).rgb+vec3(0.5))*0.5, 1.0);
    else
        color = vec4((texture(tex2, vec2(UV.x*2-1, UV.y)).rgb+vec3(0.5))*0.5, 1.0);
    /*vec2 UV_ = UV;

    int sign = -1;
    for (int i=0; i<11; ++i) {
        UV_ += sign*vec2(color.r-color.b, -color.g+color.b)*0.05;
        color += sign*texture(tex, UV_);
        sign *= -1;
    }*/
    //color /= 12.0;
}
