#pragma once
#include "itkImage.h"
#include <opencv2/opencv.hpp>
#include <string>

// Define los tipos aquí para que también estén disponibles en el .cpp
using ImageType = itk::Image<float, 3>;
using MaskType = itk::Image<unsigned char, 3>;

cv::Mat getSlice(ImageType::Pointer image, int z);
cv::Mat getMaskSlice(MaskType::Pointer mask, int z);
