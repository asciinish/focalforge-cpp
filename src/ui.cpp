#include "ui.h"
#include "ops.h"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

namespace ui {

namespace { constexpr size_t kUndoCap = 20; }

static void onTrackbar(int, void* userdata) {
    auto* st = static_cast<EditorState*>(userdata);
    st->dirty = true;
}

static void onMouse(int event, int x, int y, int, void* userdata) {
    auto* st = static_cast<EditorState*>(userdata);
    if (!st->cropMode) return;

    switch (event) {
        case cv::EVENT_LBUTTONDOWN:
            st->cropDragging = true;
            st->cropStart = {x, y};
            st->cropEnd   = {x, y};
            st->dirty = true;
            break;
        case cv::EVENT_MOUSEMOVE:
            if (st->cropDragging) {
                st->cropEnd = {x, y};
                st->dirty = true;
            }
            break;
        case cv::EVENT_LBUTTONUP:
            st->cropDragging = false;
            st->cropEnd = {x, y};
            st->dirty = true;
            break;
        default: break;
    }
}
// -------------- branch - v1.0
void setupWindow(EditorState& st) {
    cv::namedWindow(st.windowName, cv::WINDOW_AUTOSIZE);

    cv::createTrackbar("Blur",       st.windowName, &st.adj.blurRadius,    25,  onTrackbar, &st);
    cv::createTrackbar("Brightness", st.windowName, &st.adj.brightness,    200, onTrackbar, &st); 
    cv::setTrackbarMin("Brightness", st.windowName, 0); 
    cv::createTrackbar("Contrast",   st.windowName, &st.adj.contrastX100,  300, onTrackbar, &st);
    cv::createTrackbar("Saturation", st.windowName, &st.adj.saturationX100,200, onTrackbar, &st);
    cv::createTrackbar("Rotate",     st.windowName, &st.adj.rotateAngle,   360, onTrackbar, &st);
    cv::createTrackbar("Scale%",     st.windowName, &st.adj.scaleX100,     200, onTrackbar, &st);
    cv::setTrackbarMin("Scale%",     st.windowName, 10);

   
    cv::setTrackbarPos("Brightness", st.windowName, 100);
    cv::setTrackbarPos("Contrast",   st.windowName, 100);
    cv::setTrackbarPos("Saturation", st.windowName, 100);
    cv::setTrackbarPos("Rotate",     st.windowName, 180);
    cv::setTrackbarPos("Scale%",     st.windowName, 100);

    cv::setMouseCallback(st.windowName, onMouse, &st);
}

void pushUndo(EditorState& st) {
    if (st.base.empty()) return;
    st.undo.push_back(st.base.clone());
    if (st.undo.size() > kUndoCap) st.undo.pop_front();
}

bool popUndo(EditorState& st) {
    if (st.undo.empty()) return false;
    st.base = st.undo.back();
    st.undo.pop_back();
    st.dirty = true;
    return true;
}

cv::Mat renderDisplay(const EditorState& st) {
    if (st.base.empty()) return {};
    cv::Mat img = st.base.clone();

    double scale = st.adj.scaleX100 / 100.0;
    if (std::abs(scale - 1.0) > 1e-3) img = ops::resizeScale(img, scale);

    double angle = st.adj.rotateAngle - 180.0;
    if (std::abs(angle) > 0.5) img = ops::rotate(img, angle);

    if (st.adj.blurRadius > 0) img = ops::blur(img, st.adj.blurRadius);

    int    brightness = st.adj.brightness - 100;        
    double contrast   = st.adj.contrastX100 / 100.0;    
    if (brightness != 0 || std::abs(contrast - 1.0) > 1e-3)
        img = ops::brightnessContrast(img, brightness, contrast);

    double sat = st.adj.saturationX100 / 100.0;
    if (std::abs(sat - 1.0) > 1e-3) img = ops::saturation(img, sat);

    if (st.adj.grayscale) img = ops::grayscale(img);
    if (st.adj.sepia)     img = ops::sepia(img);
    if (st.adj.negate)    img = ops::negate(img);


    if (st.cropMode) {
        cv::Point a = st.cropStart;
        cv::Point b = st.cropEnd;
        cv::rectangle(img, a, b, cv::Scalar(0, 255, 0), 2);
    }

    return img;
}

} 