/**
    Shader.hpp

    @version    0.1
    @author     Miika 'LehdaRi' Lehtim�ki
    @date       2015-06-18

**/


#ifndef ROBOKASI_SHADER_HPP
#define ROBOKASI_SHADER_HPP


#include <string>
#include <GL/glew.h>


class Shader {
public:
    Shader(const std::string& vsFileName, const std::string& fsFileName);
    ~Shader(void);

    Shader(const Shader&)             = delete;
    Shader(Shader&&)                  = delete;
    Shader& operator=(const Shader&)  = delete;
    Shader& operator=(Shader&&)       = delete;

    GLuint getId(void) const;
    void use(void);

private:
    GLuint programId_;
    GLuint uniformPosition_MVP_;
    GLuint uniformPosition_Color_;
};


#endif // ROBOKASI_SHADER_HPP
