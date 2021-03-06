/**
    GLFFT.cpp

    @version    0.1
    @author     Miika 'LehdaRi' Lehtim�ki
    @date       2015-06-30
**/


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
             const std::string& vsFileName, const std::string& fftFsFileName,
             const std::string& spectrumFsFileName) :
    vertexArrayId_(0),
    arrayBufferId_(0),
    framebufferId_(0),
    textureIds_{0, 0, 0, 0},
    width_(width), height_(height),
    xDepth_(0), yDepth_(0),
    active_(0),
    fftShader_(vsFileName, fftFsFileName),
    spectrumShader_(vsFileName, spectrumFsFileName)
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

void GLFFT::operator()(GLuint srcTex1, GLuint srcTex2, GLuint destTex1, GLuint destTex2,
                       bool inverse, bool spectrum, GLuint xoffset, GLuint yoffset) {
    GLint oldViewport[4];
    glGetIntegerv(GL_VIEWPORT, oldViewport);

    glBindFramebuffer(GL_FRAMEBUFFER, framebufferId_);
    glViewport(0, 0, width_, height_);

    if (srcTex1) {
        //  copy the texture using the FBO
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, srcTex1, 0);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, textureIds_[active_], 0);
        glDrawBuffer(GL_COLOR_ATTACHMENT1);
        glBlitFramebuffer(xoffset, yoffset, xoffset+width_, yoffset+height_, 0, 0, width_, height_, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
    else {
        //  clear the active real input texture
        glReadBuffer(GL_NONE);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureIds_[active_], 0);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    if (srcTex2) {
        //  copy the texture using the FBO
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, srcTex2, 0);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, textureIds_[active_+2], 0);
        glDrawBuffer(GL_COLOR_ATTACHMENT1);
        glBlitFramebuffer(xoffset, yoffset, xoffset+width_, yoffset+height_, 0, 0, width_, height_, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
    else {
        //  clear the active imaginary input texture
        glReadBuffer(GL_NONE);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureIds_[active_+2], 0);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    //  setup framebuffer and VAO
    glDrawBuffers(2, drawBuffers__);
    glBindVertexArray(vertexArrayId_);

    //  parse spectrum
    if (inverse && spectrum) {
        glUseProgram(spectrumShader_.getId());

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureIds_[!active_], 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, textureIds_[!active_+2], 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureIds_[active_]);
        glUniform1i(glGetUniformLocation(spectrumShader_.getId(), "tex1"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureIds_[active_+2]);
        glUniform1i(glGetUniformLocation(spectrumShader_.getId(), "tex2"), 1);

        glUniform1ui(glGetUniformLocation(spectrumShader_.getId(), "inverse"), 1);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        active_ = !active_;
    }

    //  setup fft shader
    fftShader_.use();
    glUniform1ui(glGetUniformLocation(fftShader_.getId(), "xDepth"), xDepth_);
    glUniform1ui(glGetUniformLocation(fftShader_.getId(), "yDepth"), yDepth_);

    //  X-direction
    glUniform1ui(glGetUniformLocation(fftShader_.getId(), "direction"), 0);
    glUniform1i(glGetUniformLocation(fftShader_.getId(), "inverse"), inverse);
    for (auto i=0u; i<=xDepth_; ++i) {
        //  real output
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureIds_[!active_], 0);

        //  imaginary output
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, textureIds_[!active_+2], 0);

        //  real input
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureIds_[active_]);
        glUniform1i(glGetUniformLocation(fftShader_.getId(), "tex_real"), 0);

        //  imaginary input
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureIds_[active_+2]);
        glUniform1i(glGetUniformLocation(fftShader_.getId(), "tex_img"), 1);

        //  rest of the uniforms
        glUniform1ui(glGetUniformLocation(fftShader_.getId(), "iter"), i);

        //  Julie, do the thing!
        glDrawArrays(GL_TRIANGLES, 0, 6);
        active_ = !active_;
    }

    //  Y-direction
    glUniform1ui(glGetUniformLocation(fftShader_.getId(), "direction"), 1);
    for (auto i=0u; i<=yDepth_; ++i) {
        //  real output
        if (i == yDepth_ && destTex1 && !(spectrum && !inverse))
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, destTex1, 0);
        else
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureIds_[!active_], 0);

        //  imaginary output
        if (i == yDepth_ && destTex2 && !(spectrum && !inverse))
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, destTex2, 0);
        else
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, textureIds_[!active_+2], 0);

        //  real input
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureIds_[active_]);
        glUniform1i(glGetUniformLocation(fftShader_.getId(), "tex_real"), 0);

        //  imaginary input
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureIds_[active_+2]);
        glUniform1i(glGetUniformLocation(fftShader_.getId(), "tex_img"), 1);

        //  rest of the uniforms
        glUniform1ui(glGetUniformLocation(fftShader_.getId(), "iter"), i);

        //  Julie, do the thing!
        glDrawArrays(GL_TRIANGLES, 0, 6);
        active_ = !active_;
    }

    //  Form spectrum
    if (!inverse && spectrum) {
        glUseProgram(spectrumShader_.getId());

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, destTex1, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, destTex2, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureIds_[active_]);
        glUniform1i(glGetUniformLocation(spectrumShader_.getId(), "tex1"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureIds_[active_+2]);
        glUniform1i(glGetUniformLocation(spectrumShader_.getId(), "tex2"), 1);

        glUniform1ui(glGetUniformLocation(spectrumShader_.getId(), "inverse"), 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        active_ = !active_;
    }

    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //  restore old viewport
    glViewport(oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]);
    glActiveTexture(GL_TEXTURE0);
}
