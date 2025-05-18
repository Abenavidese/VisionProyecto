#ifndef THRESHOLD_H
#define THRESHOLD_H

#include <opencv2/opencv.hpp>

/**
 * @brief Aplica un filtro de umbralización a una imagen.
 *
 * @param inputImage Imagen de entrada en escala de grises.
 * @return cv::Mat Imagen binaria resultante después de aplicar el umbral.
 */
cv::Mat applyThreshold(const cv::Mat& inputImage);

#endif // THRESHOLD_H
