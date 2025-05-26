#ifndef SUAVIZADO_H
#define SUAVIZADO_H

#include <opencv2/opencv.hpp>

/**
 * @brief Aplica filtro de suavizado gaussiano a una imagen.
 * @param input Imagen de entrada (grayscale).
 * @param k Tamaño del kernel (debe ser impar).
 * @return Imagen suavizada.
 */
cv::Mat aplicarSuavizadoGaussiano(const cv::Mat& input, int k = 5);

#endif // SUAVIZADO_H
