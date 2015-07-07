/**
    TextureManipulator.cpp

    @version    0.1
    @author     Miika 'LehdaRi' Lehtimäki
    @date       2015-06-30
**/


#include "TextureManipulator.hpp"


const GLfloat TextureManipulator::quad__[] = {
    -1.0f,  -1.0f,  0.0f,
    1.0f,   -1.0f,   0.0f,
    -1.0f,  1.0f,  0.0f,
    -1.0f,  1.0f,   0.0f,
    1.0f,   -1.0f,  0.0f,
    1.0f,   1.0f,   0.0f
};


TextureManipulator::TextureManipulator(const std::string& vsFileName, const std::string& fsFileName) :
    vertexArrayId_(0),
    arrayBufferId_(0),
    framebufferId_(0),
    shader_(vsFileName, fsFileName)
{
    //  vertex array & vertex buffer for screenspace quad
    glGenVertexArrays(1, &vertexArrayId_);
    glBindVertexArray(vertexArrayId_);

    glGenBuffers(1, &arrayBufferId_);
    glBindBuffer(GL_ARRAY_BUFFER, arrayBufferId_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad__), quad__, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);

    //  framebuffer
    glGenFramebuffers(1, &framebufferId_);
}

TextureManipulator::~TextureManipulator(void) {
    if (framebufferId_)
        glDeleteFramebuffers(1, &framebufferId_);

    if (arrayBufferId_)
        glDeleteBuffers(1, &arrayBufferId_);

    if (vertexArrayId_)
        glDeleteVertexArrays(1, &vertexArrayId_);
}

void TextureManipulator::complexConjMultiply(const Texture<2>& leftTex,
                                             const Texture<2>& rightTex,
                                             Texture<2>& destTex) {
    GLint oldViewport[4];
    glGetIntegerv(GL_VIEWPORT, oldViewport);

    glBindFramebuffer(GL_FRAMEBUFFER, framebufferId_);
    glViewport(0, 0, destTex.width(), destTex.height());
    const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, drawBuffers);
    glBindVertexArray(vertexArrayId_);
    shader_.use();
    glUniform1i(glGetUniformLocation(shader_.getId(), "func"), 1);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, destTex[0], 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, destTex[1], 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, leftTex[0]);
    glUniform1i(glGetUniformLocation(shader_.getId(), "tex1"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, leftTex[1]);
    glUniform1i(glGetUniformLocation(shader_.getId(), "tex2"), 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, rightTex[0]);
    glUniform1i(glGetUniformLocation(shader_.getId(), "tex3"), 2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, rightTex[1]);
    glUniform1i(glGetUniformLocation(shader_.getId(), "tex4"), 3);

    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //  restore old viewport
    glViewport(oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]);
    glActiveTexture(GL_TEXTURE0);
}

