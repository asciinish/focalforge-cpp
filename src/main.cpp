#include "io.h"
#include "ops.h"
#include "ui.h"

#include <opencv2/highgui.hpp>
#include <iostream>
#include <sstream>

static void printHelp() {
    std::cout <<
        "\nImage Editor — keyboard controls\n"
        "  o  open image (path prompted)\n"
        "  w  write/save image (path prompted)\n"
        "  u  undo last destructive op\n"
        "  n  toggle negate\n"
        "  g  toggle grayscale\n"
        "  s  toggle sepia\n"
        "  r  rotate 90 clockwise (committed)\n"
        "  c  enter crop mode (drag with mouse, Enter to apply, Esc to cancel)\n"
        "  R  resize to exact WxH (prompted, committed)\n"
        "  C  commit current slider state into base (bakes blur/bc/sat/scale/rotate)\n"
        "  h  show this help\n"
        "  q / Esc  quit\n"
        "Sliders: Blur, Brightness, Contrast, Saturation, Rotate, Scale%\n"
        << std::flush;
}

// Commit current display (slider-adjusted) into the base image and reset sliders.
static void commitAdjustments(ui::EditorState& st) {
    ui::pushUndo(st);
    st.base = ui::renderDisplay(st);
    st.adj = ui::Adjustments{};
    // Reset trackbar positions to neutral.
    cv::setTrackbarPos("Blur",       st.windowName, 0);
    cv::setTrackbarPos("Brightness", st.windowName, 100);
    cv::setTrackbarPos("Contrast",   st.windowName, 100);
    cv::setTrackbarPos("Saturation", st.windowName, 100);
    cv::setTrackbarPos("Rotate",     st.windowName, 180);
    cv::setTrackbarPos("Scale%",     st.windowName, 100);
    st.dirty = true;
}

int main(int argc, char** argv) {
    ui::EditorState st;

    std::string initialPath;
    if (argc >= 2) {
        initialPath = argv[1];
    } else {
        initialPath = io::promptLine("Path to image: ");
    }

    st.base = io::loadImage(initialPath);
    if (st.base.empty()) {
        std::cerr << "Cannot continue without an image.\n";
        return 1;
    }

    ui::setupWindow(st);
    printHelp();

    while (true) {
        if (st.dirty) {
            st.display = ui::renderDisplay(st);
            if (!st.display.empty()) cv::imshow(st.windowName, st.display);
            st.dirty = false;
        }

        int key = cv::waitKey(20);
        if (key < 0) continue;
        key &= 0xFF;

        // Crop-mode-specific keys
        if (st.cropMode) {
            if (key == 13 /* Enter */) {
                cv::Rect r(st.cropStart, st.cropEnd);
                r = r & cv::Rect(0, 0, st.base.cols, st.base.rows);
                if (r.width > 1 && r.height > 1) {
                    // Commit slider state first so crop applies to what user sees.
                    commitAdjustments(st);
                    ui::pushUndo(st);
                    st.base = ops::crop(st.base, r);
                }
                st.cropMode = false;
                st.cropDragging = false;
                st.dirty = true;
                continue;
            }
            if (key == 27 /* Esc */) {
                st.cropMode = false;
                st.cropDragging = false;
                st.dirty = true;
                continue;
            }
        }

        switch (key) {
            case 'q':
            case 27: // Esc
                return 0;
            case 'h':
                printHelp();
                break;
            case 'o': {
                std::string p = io::promptLine("Open path: ");
                cv::Mat img = io::loadImage(p);
                if (!img.empty()) {
                    st.undo.clear();
                    st.base = img;
                    st.adj = ui::Adjustments{};
                    commitAdjustments(st); // resets sliders + redraw
                }
                break;
            }
            case 'w': {
                std::string p = io::promptLine("Save path (e.g. out.png): ");
                if (!p.empty()) io::saveImage(p, st.display);
                break;
            }
            case 'u':
                if (!ui::popUndo(st)) std::cout << "[undo] nothing to undo\n";
                break;
            case 'n':
                st.adj.negate = !st.adj.negate;
                st.dirty = true;
                break;
            case 'g':
                st.adj.grayscale = !st.adj.grayscale;
                st.dirty = true;
                break;
            case 's':
                st.adj.sepia = !st.adj.sepia;
                st.dirty = true;
                break;
            case 'r':
                commitAdjustments(st);
                ui::pushUndo(st);
                st.base = ops::rotate90CW(st.base);
                st.dirty = true;
                break;
            case 'c':
                st.cropMode = true;
                st.cropDragging = false;
                st.cropStart = st.cropEnd = {0, 0};
                std::cout << "[crop] drag a rectangle, Enter to apply, Esc to cancel\n";
                break;
            case 'C':
                commitAdjustments(st);
                std::cout << "[commit] sliders baked into base\n";
                break;
            case 'R': {
                std::string line = io::promptLine("Resize to WxH (e.g. 800x600): ");
                int w = 0, h = 0; char x = 0;
                std::istringstream iss(line);
                if ((iss >> w >> x >> h) && (x == 'x' || x == 'X') && w > 0 && h > 0) {
                    commitAdjustments(st);
                    ui::pushUndo(st);
                    st.base = ops::resizeTo(st.base, w, h);
                    st.dirty = true;
                } else {
                    std::cerr << "[resize] bad input: " << line << "\n";
                }
                break;
            }
            default: break;
        }
    }
}