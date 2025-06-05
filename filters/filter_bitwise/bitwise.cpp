#include "bitwise.h"

bool checkValidBitwiseOperation(const cv::Mat& img, const cv::Mat& mask) {
    if (img.size() != mask.size()) {
        std::cerr << "Error: Las matrices no tienen el mismo tamaño para operación bitwise." << std::endl;
        return false;
    }

    if (img.type() != mask.type()) {
        std::cerr << "Error: Las matrices no tienen el mismo tipo para operación bitwise." << std::endl;
        return false;
    }

    return true;
}

cv::Mat applyBitwiseNOT(const cv::Mat& img) {
    cv::Mat result;
    if (img.empty()) return result;
    cv::bitwise_not(img, result);
    return result;
}

cv::Mat applyBitwiseAND(const cv::Mat& img, const cv::Mat& mask) {
    cv::Mat result;
    if (!checkValidBitwiseOperation(img, mask)) return result;
    cv::bitwise_and(img, mask, result);
    return result;
}

cv::Mat applyBitwiseOR(const cv::Mat& img, const cv::Mat& mask) {
    cv::Mat result;
    if (!checkValidBitwiseOperation(img, mask)) return result;
    cv::bitwise_or(img, mask, result);
    return result;
}

cv::Mat applyBitwiseXOR(const cv::Mat& img, const cv::Mat& mask) {
    cv::Mat result;
    if (!checkValidBitwiseOperation(img, mask)) return result;
    cv::bitwise_xor(img, mask, result);
    return result;
}
