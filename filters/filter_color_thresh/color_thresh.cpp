#include "color_thresh.h"

cv::Mat aplicarColorThreshold(const cv::Mat& input, int low, int high) {
    cv::Mat output;
    cv::inRange(input, cv::Scalar(low), cv::Scalar(high), output);
    return output;
}
