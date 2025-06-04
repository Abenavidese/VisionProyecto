#include "bitwise.h"

cv::Mat applyBitwiseNOT(const cv::Mat& img) {
    cv::Mat result;
    cv::bitwise_not(img, result);
    return result;
}

cv::Mat applyBitwiseAND(const cv::Mat& img, const cv::Mat& mask) {
    cv::Mat result;
    cv::bitwise_and(img, mask, result);
    return result;
}

cv::Mat applyBitwiseOR(const cv::Mat& img, const cv::Mat& mask) {
    cv::Mat result;
    cv::bitwise_or(img, mask, result);
    return result;
}

cv::Mat applyBitwiseXOR(const cv::Mat& img, const cv::Mat& mask) {
    cv::Mat result;
    cv::bitwise_xor(img, mask, result);
    return result;
}
