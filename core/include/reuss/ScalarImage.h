#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <fmt/core.h>

namespace reuss {

struct Pixel {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} __attribute__((packed));

struct ColorLimits {
    double min;
    double max;
};

template <typename T> class ScalarImage {
    size_t width;
    size_t height;
    int colormap_ = cv::COLORMAP_JET;
    // std::vector<Pixel> pixels;

    ColorLimits clim{0, 255};
    // std::vector<T> raw_pixels; // raw data representation

    cv::Mat raw_pixels; // Raw data from detector
    cv::Mat clipped;    // Raw data but clipped to cmin, cmax
    cv::Mat normalized; // Normalized grey scale image 0-255
    cv::Mat color;      // Color version RGB only one working with applyColorMap
    cv::Mat pixels;     // Pixels to be displayed RGBA
                        // cv::Mat normalized;     //Normalized pixels 0-1

  public:
    ScalarImage(size_t width, size_t height)
        : width(width), height(height), pixels(width, height, CV_8UC3),
          raw_pixels(width, height, CV_16U), normalized(width, height, CV_8U),
          clipped(width, height, CV_16U) {
              fmt::print("Total channels: {}\n", raw_pixels.total());
              raw_pixels = 500;
              
          }

    // Pixel &operator()(size_t row, size_t col) {
    //     return pixels[col + row * width];
    // }
    // Pixel &operator[](size_t pos) { return pixels[pos]; }

    void set_colormap(int cm) { colormap_ = cm; }
    void set_clim(double min, double max) {
        clim.min = min;
        clim.max = max;
    }

    uint8_t *data() { return reinterpret_cast<uint8_t *>(pixels.ptr()); }
    uint8_t *raw_data() {
        return reinterpret_cast<uint8_t *>(raw_pixels.ptr());
    }

    void map() {
        raw_pixels.copyTo(clipped);
        clip(clipped);
        cv::normalize(clipped, normalized, 0., 255., cv::NORM_MINMAX, CV_8U);
        cv::applyColorMap(normalized, pixels, colormap_);
    }

    void clip(cv::Mat &img) {
        // Clip high and low values
        for (int i = 0; i != img.total(); ++i) {
            if (auto &val = img.at<uint16_t>(i); val > clim.max) {
                val = clim.max;
            } else if (val < clim.min) {
                val = clim.min;
            }
        }
    }

    void set_alpha(uint8_t alpha) {

        // for (int i = 0; i != pixels.total(); ++i) {
        //     pixels.at<Pixel>(i).a = alpha;
        // }
        // for (auto &p : pixels)
        //     p.a = alpha;
    }
};
} // namespace reuss