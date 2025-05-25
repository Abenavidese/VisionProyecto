#ifndef PIXEL_OPS_H
#define PIXEL_OPS_H

#include <opencv2/opencv.hpp>

/**
 * @brief Aplica inversión de intensidad (negativo) a una imagen.
 * @param input Imagen de entrada (grayscale).
 * @return Imagen con píxeles invertidos.
 */
cv::Mat aplicarInversion(const cv::Mat& input);

#endif // PIXEL_OPS_H
