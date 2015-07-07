/**
    FS_Texture.glsl

    @version    0.1
    @author     Miika 'LehdaRi' Lehtimäki
    @date       2015-04-18
**/


#version 330 core


#define PI 3.14159265359


in vec2 UV;

out vec4 color;

uniform sampler2D tex1;
uniform sampler2D tex2;

void main() {
    if (UV.x < 0.5)
        color = pow(vec4(texture(tex1, vec2(UV.x*2, UV.y)).rgb, 1.0), vec4(0.5, 0.5, 0.5, 1.0));
    else
        color = vec4(texture(tex2, vec2(UV.x*2-1, UV.y)).rgb, 1.0);
}
