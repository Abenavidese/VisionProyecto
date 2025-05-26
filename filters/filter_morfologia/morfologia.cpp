#include "morfologia.h"

cv::Mat aplicarDilatacion(const cv::Mat& input, int kernelSize) {
    cv::Mat output;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT,
                                               cv::Size(kernelSize, kernelSize));
    cv::dilate(input, output, kernel);
    return output;
}
