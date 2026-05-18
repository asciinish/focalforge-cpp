#pragma once 
#include <opencv2/core.hpp>
#include <string>

namespace io { 
    
cv::Mat loadImage(const std::string& path);
bool    saveImage(const std::string& path, const cv::Mat& image);
std::string promptLine(const std::string& message);

    
}