#include "filter_bilateral.h"

cv::Mat aplicarFiltroBilateral(const cv::Mat& input) {
    cv::Mat output;
    cv::bilateralFilter(input, output, 9, 75, 75);  // parámetros: diámetro, sigmaColor, sigmaSpace
    return output;
}
