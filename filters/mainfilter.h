#ifndef MAINFILTER_H
#define MAINFILTER_H

#include <opencv2/opencv.hpp>
#include "filter_thresholding/threshold.h"

// Exportar la función de umbralización
cv::Mat applyThreshold(const cv::Mat& inputImage);

#endif // MAINFILTER_H
