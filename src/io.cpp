#include "io.h"
#include <opencv2/imgcodecs.hpp>
#include <iostream>

namespace io {

cv::Mat loadImage(const std::string& path) {
    cv::Mat img = cv::imread(path, cv::IMREAD_COLOR);
    if (img.empty()) {
        std::cerr << "[io] Failed to load image: " << path << "\n";
    }
    return img;
}

bool saveImage(const std::string& path, const cv::Mat& img) {
    if (img.empty()) {
        std::cerr << "[io] Refusing to save empty image\n";
        return false;
    }
    bool ok = cv::imwrite(path, img);
    if (!ok) std::cerr << "[io] Failed to save image: " << path << "\n";
    else     std::cout << "[io] Saved: " << path << "\n";
    return ok;
}

std::string promptLine(const std::string& message) {
    std::cout << message << std::flush;
    std::string line;
    std::getline(std::cin, line);
    return line;
}

} 