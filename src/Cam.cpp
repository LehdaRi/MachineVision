/**
    Cam.cpp

    @version    0.1
    @author     Miika 'LehdaRi' Lehtimäki
    @date       2015-06-30
**/


#include "Cam.hpp"

#include <iostream>


Cam::Cam(int camId, double frameWidth, double frameHeight, double fps) :
    cap_(camId),
    frameWidth_(0), frameHeight_(0)
{
    if (!cap_.isOpened()) {
        std::cerr << "Cannot open the video cam" << std::endl;
        return;
    }

    cap_.set(CV_CAP_PROP_FRAME_WIDTH, frameWidth);
    cap_.set(CV_CAP_PROP_FRAME_HEIGHT, frameHeight);
    //cap.set(CV_CAP_PROP_FOURCC,CV_FOURCC('M','J','P','G'));
    cap_.set(CV_CAP_PROP_FPS, fps);

    frameWidth_ = cap_.get(CV_CAP_PROP_FRAME_WIDTH);
    frameHeight_ = cap_.get(CV_CAP_PROP_FRAME_HEIGHT);

    frameData_.resize(0, frameWidth_ * frameHeight_ * 3);
    int fs[] = {(int)frameWidth_, (int)frameHeight_};

    frame_ = cv::Mat(2, fs, CV_8UC3, &frameData_[0]);
    if (!cap_.read(frame_)) {
        std::cerr << "Cannot read a frame from video stream" << std::endl;
        return;
    }

    texture_ = Texture<>(frameWidth_, frameHeight_, GL_RGB, GL_REPEAT, GL_REPEAT, frame_.data, GL_BGR);
}

void Cam::read(void) {
    //  read frame from webcam
    glBindTexture(GL_TEXTURE_2D, texture_[0]);
    if (!cap_.read(frame_)) { // read a new frame from video
        std::cerr << "Cannot read a frame from video stream" << std::endl;
        return;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frameWidth_, frameHeight_, 0,
                 GL_BGR, GL_UNSIGNED_BYTE, frame_.data);
}

unsigned Cam::width(void) const {
    return frameWidth_;
}

unsigned Cam::height(void) const {
    return frameHeight_;
}

const Texture<>& Cam::texture(void) const {
    return texture_;
}
