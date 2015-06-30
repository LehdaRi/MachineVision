/**
    FS_TextureManip.glsl

    @version    0.1
    @author     Miika 'LehdaRi' Lehtimäki
    @date       2015-06-30
**/


#version 330 core


#define PI 3.14159265359


in vec2 UV;

out vec4 color;

uniform sampler2D tex1;
uniform sampler2D tex2;

void main() {
    color = texture(tex1, UV) * texture(tex2, UV);
}

