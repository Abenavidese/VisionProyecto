#ifndef BITWISE_H
#define BITWISE_H

#include <opencv2/opencv.hpp>

cv::Mat applyBitwiseNOT(const cv::Mat& img);
cv::Mat applyBitwiseAND(const cv::Mat& img, const cv::Mat& mask);
cv::Mat applyBitwiseOR(const cv::Mat& img, const cv::Mat& mask);
cv::Mat applyBitwiseXOR(const cv::Mat& img, const cv::Mat& mask);

#endif // BITWISE_H
