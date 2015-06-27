#include "GLFFT.hpp"

#include <iostream>


const GLfloat GLFFT::quad__[] = {
    -1.0f,  -1.0f,  0.0f,
    1.0f,   -1.0f,   0.0f,
    -1.0f,  1.0f,  0.0f,
    -1.0f,  1.0f,   0.0f,
    1.0f,   -1.0f,  0.0f,
    1.0f,   1.0f,   0.0f
};

const GLenum GLFFT::drawBuffers__[] = {
    GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1
};


GLFFT::GLFFT(unsigned width, unsigned height,
             const std::string& vsFileName, const std::string& fsFileName) :
    vertexArrayId_(0),
    arrayBufferId_(0),
    framebufferId_(0),
    textureIds_{0, 0, 0, 0},
    width_(width), height_(height),
    xDepth_(0), yDepth_(0),
    active_(0),
    shader_(vsFileName, fsFileName)
{
    //  check width & height
    for (auto i=0u; i<32; ++i) {
        if (width_ >> i & 0x1) {
            if (xDepth_)
                std::cerr << "Invalid FFT width (must be power of 2)" << std::endl;
            else
                xDepth_ = i;
        }
        if (height_ >> i & 0x1) {
            if (yDepth_)
                std::cerr << "Invalid FFT width (must be power of 2)" << std::endl;
            else
                yDepth_ = i;
        }
    }

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

    //  textures
    glGenTextures(4, textureIds_);
    for (auto i=0; i<4; ++i) {
        glBindTexture(GL_TEXTURE_2D, textureIds_[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width_, height_, 0, GL_RGBA, GL_FLOAT, (void*)0);
    }
}

GLFFT::~GLFFT(void) {
    if (textureIds_[0] && textureIds_[1])
        glDeleteTextures(2, textureIds_);

    if (framebufferId_)
        glDeleteFramebuffers(1, &framebufferId_);

    if (arrayBufferId_)
        glDeleteBuffers(1, &arrayBufferId_);

    if (vertexArrayId_)
        glDeleteVertexArrays(1, &vertexArrayId_);
}

GLuint GLFFT::transform(GLuint textureId, GLuint xoffset, GLuint yoffset) {
    GLint oldViewport[4];
    glGetIntegerv(GL_VIEWPORT, oldViewport);

    //  copy the texture using the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferId_);
    glViewport(0, 0, width_, height_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, textureIds_[active_], 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT1);
    glBlitFramebuffer(xoffset, yoffset, xoffset+width_, yoffset+height_, 0, 0, width_, height_, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    //  setup framebuffer, VAO and shader
    glReadBuffer(GL_NONE);
    glDrawBuffers(2, drawBuffers__);
    glBindVertexArray(vertexArrayId_);
    shader_.use();
    glUniform1ui(glGetUniformLocation(shader_.getId(), "xDepth"), xDepth_);
    glUniform1ui(glGetUniformLocation(shader_.getId(), "yDepth"), yDepth_);

    //  X-direction
    glUniform1ui(glGetUniformLocation(shader_.getId(), "direction"), 0);
    for (auto i=0u; i<1; ++i) {
        //  real output
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureIds_[!active_], 0);
        //  imaginary output
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, textureIds_[!active_+2], 0);
        //  real input
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureIds_[active_]);
        glUniform1i(glGetUniformLocation(shader_.getId(), "tex_real"), 0);
        //  imaginary input
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureIds_[active_+2]);
        glUniform1i(glGetUniformLocation(shader_.getId(), "tex_img"), 1);

        //  rest of the uniforms
        glUniform1ui(glGetUniformLocation(shader_.getId(), "iter"), i);

        //  Julie, do the thing!
        glDrawArrays(GL_TRIANGLES, 0, 6);
        active_ = !active_;
    }

    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //  restore old viewport
    glViewport(oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]);
    glActiveTexture(GL_TEXTURE0);

    for (auto i=0; i<4; ++i)
        printf("%u ", textureIds_[i]);
    printf("\n");

    return textureIds_[active_];
}
