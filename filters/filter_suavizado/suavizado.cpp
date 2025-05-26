#include "suavizado.h"

cv::Mat aplicarSuavizadoGaussiano(const cv::Mat& input, int k) {
    cv::Mat output;
    cv::GaussianBlur(input, output, cv::Size(k, k), 0);
    return output;
}
