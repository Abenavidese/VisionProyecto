#ifndef BORDES_H
#define BORDES_H

#include <opencv2/opencv.hpp>

/**
 * @brief Aplica detección de bordes (Canny) a una imagen en escala de grises.
 * @param input Imagen de entrada (cv::Mat).
 * @return Imagen con bordes detectados.
 */
cv::Mat aplicarDeteccionBordes(const cv::Mat& input);

#endif // BORDES_H
