/**
    Texture.hpp

    @version    0.1
    @author     Miika 'LehdaRi' Lehtimäki
    @date       2015-06-30
**/


#ifndef TEXTURE_HPP
#define TEXTURE_HPP


#include <GL/glew.h>
#include <SFML/Graphics/Image.hpp>


template<unsigned S = 1u>
class Texture {
public:
    Texture(void);
    Texture(unsigned width, unsigned height,
            GLenum format = GL_RGBA, GLenum wrapS = GL_REPEAT, GLenum wrapT = GL_REPEAT,
            void* data = nullptr, GLenum dataFormat = GL_RGBA, GLenum dataType = GL_UNSIGNED_BYTE);
    Texture(const std::string& fileName,
            GLenum format = GL_RGBA, GLenum wrapS = GL_REPEAT, GLenum wrapT = GL_REPEAT);
    ~Texture(void);

    Texture(Texture&&);
    Texture<S>& operator=(Texture&&);
    Texture(const Texture& other)               = delete;
    Texture<S>& operator=(const Texture& other) = delete;

    GLuint operator[](unsigned id) const;

    unsigned width(void) const;
    unsigned height(void) const;

    void loadFromFile(const std::string& fileName, unsigned id = 0u);

private:
    GLuint textureIds_[S];
    unsigned width_, height_;
};


//  Member definitions
template<unsigned S>
Texture<S>::Texture(void) :
    textureIds_{0},
    width_(0), height_(0)
{}

template<unsigned S>
Texture<S>::Texture(unsigned width, unsigned height,
                    GLenum format, GLenum wrapS, GLenum wrapT,
                    void* data, GLenum dataFormat, GLenum dataType) :
    textureIds_{0},
    width_(width), height_(height)
{
    glGenTextures(S, textureIds_);
    for (auto i=0u; i<S; ++i) {
        glBindTexture(GL_TEXTURE_2D, textureIds_[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width_, height_, 0, dataFormat, dataType, data);
    }
}

template<unsigned S>
Texture<S>::Texture(const std::string& fileName,
                    GLenum format, GLenum wrapS, GLenum wrapT) :
    textureIds_{0},
    width_(0), height_(0)
{
    sf::Image img;
    img.loadFromFile(fileName);
    auto is = img.getSize();
    width_ = is.x;
    height_ = is.y;

    glGenTextures(S, textureIds_);
    for (auto i=0u; i<S; ++i) {
        glBindTexture(GL_TEXTURE_2D, textureIds_[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());
    }
}

template<unsigned S>
Texture<S>::~Texture(void) {
    if (textureIds_[0])
        glDeleteTextures(S, textureIds_);
}

template<unsigned S>
Texture<S>::Texture(Texture&& other) {
    *this = other;
}

template<unsigned S>
Texture<S>& Texture<S>::operator=(Texture<S>&& other) {
    if (textureIds_[0])
        glDeleteTextures(S, textureIds_);

    for (auto i=0u; i<S; ++i) {
        textureIds_[i] = other.textureIds_[i];
        other.textureIds_[i] = 0;
    }

    width_ = other.width_;
    height_ = other.height_;

    return *this;
}

template<unsigned S>
GLuint Texture<S>::operator[](unsigned id) const {
    if (id >= S)
        return 0;
    else
        return textureIds_[id];
}

template<unsigned S>
unsigned Texture<S>::width(void) const {
    return width_;
}

template<unsigned S>
unsigned Texture<S>::height(void) const {
    return height_;
}

template<unsigned S>
void Texture<S>::loadFromFile(const std::string& fileName, unsigned id) {
    if (id >= S)
        return;

    sf::Image img;
    img.loadFromFile(fileName);
    auto is = img.getSize();
    if (is.x != width_ || is.y != height_)
        return;

    glBindTexture(GL_TEXTURE_2D, textureIds_[id]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, is.x, is.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());
}


#endif  //  TEXTURE_HPP
