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

