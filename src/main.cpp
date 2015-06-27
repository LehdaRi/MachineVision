#include "opencv2/highgui/highgui.hpp"
#include "GLFFT.hpp"

#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include <iostream>
#include <vector>


using namespace cv;
using namespace std;


int main(int argc, char* argv[])
{
/*
    VideoCapture cap(0); // open the video camera no. 0

    if (!cap.isOpened()) { // if not success, exit program
        cout << "Cannot open the video cam" << endl;
        return -1;
    }

    cap.set(CV_CAP_PROP_FRAME_WIDTH, 1920.0/2);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 1080.0/2);
    cap.set(CV_CAP_PROP_FOURCC,CV_FOURCC('M','J','P','G'));
    cap.set(CV_CAP_PROP_FPS, 30.0);

   int dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
   int dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

    std::vector<uint8_t> frameData(0, dWidth * dHeight * 3);
    const int frameSize[] = {dWidth, dHeight};

    Mat frame(2, frameSize, CV_8UC3, &frameData[0]);
    if (!cap.read(frame)) { // read a new frame from video
        cout << "Cannot read a frame from video stream" << endl;
        return -1;
    }
*/
    //  TEMP
    sf::Image testImg;
    testImg.loadFromFile("res/test4.png");

    sf::Window window(sf::VideoMode(512, 512), "Vision");
    window.setFramerateLimit(30);
    window.setActive();

    if (glewInit() != GLEW_OK) {
        cout << "Unable to init GLEW" << endl;
        return -1;
    }

    const GLfloat quad[18] = {
        -1.0f,  -1.0f,  0.0f,
        1.0f,   -1.0f,   0.0f,
        -1.0f,  1.0f,  0.0f,
        -1.0f,  1.0f,   0.0f,
        1.0f,   -1.0f,  0.0f,
        1.0f,   1.0f,   0.0f
    };

    GLuint vertexArrayId;
    glGenVertexArrays(1, &vertexArrayId);
    glBindVertexArray(vertexArrayId);

    GLuint bufferId;
    glGenBuffers(1, &bufferId);
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);

    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dWidth, dHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, frame.data);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, testImg.getPixelsPtr());

    Shader shader("src/VS_Texture.glsl", "src/FS_Texture.glsl");
    GLFFT fft(8, 8, "src/VS_FFT.glsl", "src/FS_FFT.glsl");

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
        glBindTexture(GL_TEXTURE_2D, textureId);
        /*if (!cap.read(frame)) { // read a new frame from video
            cout << "Cannot read a frame from video stream" << endl;
            return -1;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dWidth, dHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, frame.data);
*/
        //  fft
        //GLuint fftTexId = fft.transform(textureId, 256, 32);
        GLuint fftTexId = fft.transform(textureId);

        //  draw
        glBindVertexArray(vertexArrayId);
        shader.use();
        glBindTexture(GL_TEXTURE_2D, fftTexId);
        glUniform1i(glGetUniformLocation(shader.getId(), "tex"), 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // End the current frame and display its contents on screen
        window.display();
    }

    glDeleteTextures(1, &textureId);
    glDeleteBuffers(1, &bufferId);
    glDeleteVertexArrays(1, &vertexArrayId);

    return 0;

}
