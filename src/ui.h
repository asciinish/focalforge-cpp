#pragma once 
#include <opencv2/core.hpp>
#include <deque>
#include <string>

namspace ui {

struct Adjustments { 
    int blurRadius = 0;
    int brightness = 0;
    int contrastX100 = 100;
    int saturationX100 = 100;
    int rotateAngle = 180;
    int scaleX100 = 100;
    bool negate = false;
    bool grayscale = false;
    bool sepia = false;
};

struct EditorState {
    cv::Mat base;
    cv::Mat dispaly;
    Adjustments adj;
    std::deque<cv::Mat> undo;
    std::string windowName = "Image Editor";
    bool dirty = true;
    
    // crop state
    
    bool cropMode = false;
    bool cropDragging = false;
    cv::Point cropStart;
    cv::Point cropEnd;
};