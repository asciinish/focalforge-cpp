#include "ops.h"
#include <opencv2/imgproc.hpp>
#include <algorithm>
#include <cmath>

namespace ops {

cv::Mat blur(const cv::Mat& src, int radius) {
    if (radius <= 0 || src.empty()) return src.clone();
    int k = radius * 2 + 1; 
    cv::Mat dst;
    cv::GaussianBlur(src, dst, cv::Size(k, k), 0.0);
    return dst;
}

cv::Mat negate(const cv::Mat& src) {
    if (src.empty()) return {};
    cv::Mat dst;
    cv::bitwise_not(src, dst);
    return dst;
}

cv::Mat brightnessContrast(const cv::Mat& src, int brightness, double contrast) {
    if (src.empty()) return {};
    cv::Mat dst;
    // dst = src * contrast + brightness, saturated to 8-bit
    src.convertTo(dst, -1, contrast, static_cast<double>(brightness));
    return dst;
}

cv::Mat saturation(const cv::Mat& src, double scale) {
    if (src.empty()) return {};
    if (src.channels() == 1) return src.clone();

    cv::Mat hsv;
    cv::cvtColor(src, hsv, cv::COLOR_BGR2HSV);

    std::vector<cv::Mat> ch;
    cv::split(hsv, ch);
    ch[1].convertTo(ch[1], CV_8U, scale, 0.0);
    cv::merge(ch, hsv);

    cv::Mat dst;
    cv::cvtColor(hsv, dst, cv::COLOR_HSV2BGR);
    return dst;
}

cv::Mat grayscale(const cv::Mat& src) {
    if (src.empty()) return {};
    if (src.channels() == 1) return src.clone();
    cv::Mat g, dst;
    cv::cvtColor(src, g, cv::COLOR_BGR2GRAY);
    cv::cvtColor(g, dst, cv::COLOR_GRAY2BGR);
    return dst;
}

cv::Mat sepia(const cv::Mat& src) {
    if (src.empty()) return {};
    cv::Mat input = src;
    if (src.channels() == 1) {
        cv::cvtColor(src, input, cv::COLOR_GRAY2BGR);
    }
    
    cv::Matx33f kernel(
        0.272f, 0.534f, 0.131f,  // B
        0.349f, 0.686f, 0.168f,  // G
        0.393f, 0.769f, 0.189f); // R
    cv::Mat dst;
    cv::transform(input, dst, kernel);
    return dst;
}

cv::Mat rotate(const cv::Mat& src, double angleDeg) {
    if (src.empty()) return {};
    cv::Point2f center(src.cols / 2.0f, src.rows / 2.0f);
    cv::Mat M = cv::getRotationMatrix2D(center, angleDeg, 1.0);

    
    cv::Rect2f bbox =
        cv::RotatedRect(cv::Point2f(), src.size(), static_cast<float>(angleDeg))
            .boundingRect2f();
    M.at<double>(0, 2) += bbox.width / 2.0 - center.x;
    M.at<double>(1, 2) += bbox.height / 2.0 - center.y;

    cv::Mat dst;
    cv::warpAffine(src, dst, M, bbox.size(), cv::INTER_LINEAR,
                   cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
    return dst;
}

cv::Mat rotate90CW(const cv::Mat& src) {
    if (src.empty()) return {};
    cv::Mat dst;
    cv::rotate(src, dst, cv::ROTATE_90_CLOCKWISE);
    return dst;
}

cv::Mat crop(const cv::Mat& src, const cv::Rect& r) {
    if (src.empty()) return {};
    cv::Rect bounds(0, 0, src.cols, src.rows);
    cv::Rect safe = r & bounds;
    if (safe.width <= 0 || safe.height <= 0) return src.clone();
    return src(safe).clone();
}

cv::Mat resizeScale(const cv::Mat& src, double scale) {
    if (src.empty() || scale <= 0.0) return {};
    cv::Mat dst;
    cv::resize(src, dst, cv::Size(), scale, scale,
               scale < 1.0 ? cv::INTER_AREA : cv::INTER_LINEAR);
    return dst;
}

cv::Mat resizeTo(const cv::Mat& src, int width, int height) {
    if (src.empty() || width <= 0 || height <= 0) return {};
    cv::Mat dst;
    bool shrinking = width * height < src.cols * src.rows;
    cv::resize(src, dst, cv::Size(width, height), 0, 0,
               shrinking ? cv::INTER_AREA : cv::INTER_LINEAR);
    return dst;
}

} 