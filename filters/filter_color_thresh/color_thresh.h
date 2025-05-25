#ifndef COLOR_THRESH_H
#define COLOR_THRESH_H

#include <opencv2/opencv.hpp>

/**
 * @brief Aplica binarización por umbral de color a una imagen.
 * @param input Imagen de entrada en escala de grises.
 * @param low Límite inferior del umbral.
 * @param high Límite superior del umbral.
 * @return Imagen binarizada entre low y high.
 */
cv::Mat aplicarColorThreshold(const cv::Mat& input, int low = 100, int high = 200);

#endif // COLOR_THRESH_H
