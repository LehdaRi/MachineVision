/**
    Cam.hpp

    @version    0.1
    @author     Miika 'LehdaRi' Lehtimäki
    @date       2015-06-30
**/


#ifndef CAM_HPP
#define CAM_HPP


#include "opencv2/highgui/highgui.hpp"

#include "Texture.hpp"


class Cam {
public:
    Cam(int camId, double frameWidth, double frameHeight, double fps);

    void read(void);

    unsigned width(void) const;
    unsigned height(void) const;
    const Texture<>& texture(void) const;

private:
    cv::VideoCapture cap_;
    cv::Mat frame_;
    unsigned frameWidth_, frameHeight_;
    std::vector<uint8_t> frameData_;

    Texture<> texture_;
};


#endif //  CAM_HPP
