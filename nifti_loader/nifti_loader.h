#pragma once
#include "itkImage.h"
#include <string>

using ImageType = itk::Image<float, 3>;
using MaskType = itk::Image<unsigned char, 3>;

ImageType::Pointer loadImage(const std::string& filename);
MaskType::Pointer loadMask(const std::string& filename);
