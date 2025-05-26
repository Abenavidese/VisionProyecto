#ifndef MORFOLOGIA_H
#define MORFOLOGIA_H

#include <opencv2/opencv.hpp>

/**
 * @brief Aplica dilatación morfológica a una imagen binaria.
 * @param input Imagen binaria (máscara del tumor).
 * @param kernelSize Tamaño del elemento estructurante (debe ser impar).
 * @return Imagen dilatada.
 */
cv::Mat aplicarDilatacion(const cv::Mat& input, int kernelSize = 3);

#endif // MORFOLOGIA_H
