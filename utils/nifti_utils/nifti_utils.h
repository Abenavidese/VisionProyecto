#ifndef NIFTI_UTILS_H
#define NIFTI_UTILS_H

#include <QString>
#include <vector>
#include <opencv2/opencv.hpp>

std::vector<cv::Mat> loadNiftiToCvSlices(const QString& path);

#endif // NIFTI_UTILS_H
