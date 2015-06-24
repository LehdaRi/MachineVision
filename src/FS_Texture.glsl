/**
    FS_Texture.glsl

    @version    0.1
    @author     Miika 'LehdaRi' Lehtimäki
    @date       2015-04-18
**/


#version 330 core

in vec2 UV;

out vec4 color;

uniform sampler2D tex;

void main() {
    color = texture(tex, UV);
    color += texture(tex, UV+vec2(0.1, 0.0));
    color += texture(tex, UV+vec2(-0.1, 0.0));
    color += texture(tex, UV+vec2(0.0, 0.1));
    color += texture(tex, UV+vec2(0.0, -0.1));
    color *= 0.2;
}
