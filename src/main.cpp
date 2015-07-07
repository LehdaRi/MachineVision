#include "Cam.hpp"
#include "Texture.hpp"
#include "TextureManipulator.hpp"
#include "GLFFT.hpp"

#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include <iostream>
#include <vector>


#define QUAD(xPos, yPos, xSize, ySize) xPos, yPos, 0.0f,\
                                       xPos+xSize, yPos, 0.0f,\
                                       xPos, yPos+ySize, 0.0f,\
                                       xPos, yPos+ySize, 0.0f,\
                                       xPos+xSize, yPos, 0.0f,\
                                       xPos+xSize, yPos+ySize, 0.0f\


using namespace std;


int main(int argc, char* argv[])
{
    sf::Window window(sf::VideoMode(1024, 512), "Vision");
    window.setFramerateLimit(30);
    window.setActive();

    if (glewInit() != GLEW_OK) {
        cout << "Unable to init GLEW" << endl;
        return -1;
    }

    //Cam cam1(0, 1920.0/2, 1080.0/2, 30.0);
    //Cam cam2(1, 1920.0/2, 1080.0/2, 30.0);

    const GLfloat quads[] = {
        QUAD(-1.0f, -1.0f, 2.0f, 2.0f)
    };

    GLuint vertexArrayId;
    glGenVertexArrays(1, &vertexArrayId);
    glBindVertexArray(vertexArrayId);

    GLuint bufferId;
    glGenBuffers(1, &bufferId);
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quads), quads, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);

    Texture<2> tex1(512, 512, GL_RGBA32F);
    Texture<2> tex2(512, 512, GL_RGBA32F);
    Texture<2> tex3(512, 512, GL_RGBA32F);

    Texture<> test1("res/test7a.png");
    Texture<> test2("res/test7b.png");
    //Texture<1> filter("res/filter2.png");

    Shader shader("src/VS_Texture.glsl", "src/FS_Texture.glsl");
    GLFFT fft(512, 512, "src/VS_UV.glsl", "src/FS_FFT.glsl", "src/FS_Spectrum.glsl");
    TextureManipulator texMan("src/VS_UV.glsl", "src/FS_TextureManip.glsl");

    // The main loop - ends as soon as the window is closed
    while (window.isOpen())
    {
        // Event processing
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Request for closing the window
            if (event.type == sf::Event::Closed)
                window.close();
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //  read frame from webcam
        //cam1.read();
        //cam2.read();

        //  fft
        //fft(cam1.texture()[0], 0, tex1[0], tex1[1], false, false, 256, 32);
        //fft(cam2.texture()[0], 0, tex2[0], tex2[1], false, false, 256, 32);
        fft(test1[0], 0, tex1[0], tex1[1], false, false);
        fft(test2[0], 0, tex2[0], tex2[1], false, false);

        texMan.complexConjMultiply(tex1, tex2, tex3);

        fft(tex3[0], tex3[1], tex3[0], tex3[1], true, false);

        //  draw
        glBindVertexArray(vertexArrayId);
        shader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex3[0]);
        glUniform1i(glGetUniformLocation(shader.getId(), "tex1"), 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex3[1]);
        glUniform1i(glGetUniformLocation(shader.getId(), "tex2"), 1);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // End the current frame and display its contents on screen
        window.display();
    }

    glDeleteBuffers(1, &bufferId);
    glDeleteVertexArrays(1, &vertexArrayId);

    return 0;

}
