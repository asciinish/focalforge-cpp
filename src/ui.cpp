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