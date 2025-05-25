#include "bordes.h"

cv::Mat aplicarDeteccionBordes(const cv::Mat& input) {
    cv::Mat edges;
    cv::Canny(input, edges, 100, 200);  // Umbrales ajustables
    return edges;
}
