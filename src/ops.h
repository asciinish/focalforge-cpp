#pragma once
#include <opencv2/core.hpp>

namespace ops {


cv::Mat blur(const cv::Mat& src, int radius);                // radius in [0, 25]
cv::Mat negate(const cv::Mat& src);                          // per-channel invert
cv::Mat brightnessContrast(const cv::Mat& src,
                           int brightness,                   // [-100, 100]
                           double contrast);                 // [0.0, 3.0]
cv::Mat saturation(const cv::Mat& src, double scale);        // [0.0, 2.0]
cv::Mat grayscale(const cv::Mat& src);                       
cv::Mat rotate(const cv::Mat& src, double angleDeg);        
cv::Mat rotate90CW(const cv::Mat& src);
cv::Mat crop(const cv::Mat& src, const cv::Rect& r);         
cv::Mat resizeScale(const cv::Mat& src, double scale);       
cv::Mat resizeTo(const cv::Mat& src, int width, int height);

} 