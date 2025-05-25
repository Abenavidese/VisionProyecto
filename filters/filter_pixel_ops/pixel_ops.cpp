#include "pixel_ops.h"

cv::Mat aplicarInversion(const cv::Mat& input) {
    cv::Mat output;
    cv::bitwise_not(input, output);
    return output;
}
