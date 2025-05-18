#include "threshold.h"

cv::Mat applyThreshold(const cv::Mat& inputImage) {
    cv::Mat outputImage;

    // Aplicar umbralización binaria
    // - Umbral: 128
    // - Valor máximo: 255
    cv::threshold(inputImage, outputImage, 128, 255, cv::THRESH_BINARY);

    return outputImage;
}
