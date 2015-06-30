/**
    TextureManipulator.hpp

    @version    0.1
    @author     Miika 'LehdaRi' Lehtimäki
    @date       2015-06-30
**/


#ifndef TEXTUREMANIPULATOR_HPP
#define TEXTUREMANIPULATOR_HPP


#include "Shader.hpp"
#include "Texture.hpp"

#include <GL/glew.h>
#include <string>


constexpr unsigned min(unsigned a, unsigned b) {
    return a < b ? b : a;
}


class TextureManipulator {
public:
    TextureManipulator(const std::string& vsFileName, const std::string& fsFileName);
    ~TextureManipulator(void);

    TextureManipulator(const TextureManipulator&)             = delete;
    TextureManipulator(TextureManipulator&&)                  = delete;
    TextureManipulator& operator=(const TextureManipulator&)  = delete;
    TextureManipulator& operator=(TextureManipulator&&)       = delete;

    template <unsigned S1, unsigned S2>
    void multiply(const Texture<S1>& srcTex1, const Texture<S2>& srcTex2,
                  Texture<min(S1, S2)>& destTex);

private:
    GLuint vertexArrayId_;
    GLuint arrayBufferId_;
    GLuint framebufferId_;
    Shader shader_;

    static const GLfloat quad__[18];
};


//  Member definitions
template <unsigned S1, unsigned S2>
void TextureManipulator::multiply(const Texture<S1>& srcTex1, const Texture<S2>& srcTex2,
                                  Texture<min(S1, S2)>& destTex) {
    if (srcTex1.width() != destTex.width() || srcTex2.width() != destTex.width() ||
        srcTex1.height() != destTex.height() || srcTex2.height() != destTex.height())
        return;

    GLint oldViewport[4];
    glGetIntegerv(GL_VIEWPORT, oldViewport);

    glBindFramebuffer(GL_FRAMEBUFFER, framebufferId_);
    glViewport(0, 0, destTex.width(), destTex.height());
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glBindVertexArray(vertexArrayId_);
    shader_.use();

    for (auto i=0u; i<min(S1, S2); ++i) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, destTex[i], 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, srcTex1[i]);
        glUniform1i(glGetUniformLocation(shader_.getId(), "tex1"), 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, srcTex2[i]);
        glUniform1i(glGetUniformLocation(shader_.getId(), "tex2"), 1);

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //  restore old viewport
    glViewport(oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]);
    glActiveTexture(GL_TEXTURE0);
}


#endif  //  TEXTUREMANIPULATOR_HPP

