#include "contrast.h"

cv::Mat aplicarContrastStretching(const cv::Mat& src) {
    cv::Mat gray;
    if (src.channels() > 1)
        cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    else
        gray = src.clone();

    double minVal, maxVal;
    cv::minMaxLoc(gray, &minVal, &maxVal);

    // Escalar a [0, 255]
    cv::Mat stretched;
    gray.convertTo(stretched, CV_8U, 255.0 / (maxVal - minVal), -minVal * 255.0 / (maxVal - minVal));

    return stretched;
}
