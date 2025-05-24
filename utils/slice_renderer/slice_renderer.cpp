#include "slice_renderer.h"
#include "filter_thresholding/threshold.h"

QImage renderOriginal(const cv::Mat& slice) {
    cv::Mat resized;
    cv::resize(slice, resized, cv::Size(300, 300));
    return QImage(resized.data, resized.cols, resized.rows, resized.step, QImage::Format_Grayscale8).copy();
}

QImage renderOverlay(const cv::Mat& slice, const cv::Mat& mask, bool showMask) {
    cv::Mat color;
    cv::cvtColor(slice, color, cv::COLOR_GRAY2BGR);
    if (showMask) {
        for (int y = 0; y < mask.rows; ++y)
            for (int x = 0; x < mask.cols; ++x)
                if (mask.at<uchar>(y, x) > 0)
                    color.at<cv::Vec3b>(y, x) = {0, 0, 255};
    }
    cv::resize(color, color, cv::Size(300, 300));
    return QImage(color.data, color.cols, color.rows, color.step, QImage::Format_RGB888).copy();
}

QImage renderTumorOnly(const cv::Mat& mask, bool onlyTumor) {
    cv::Mat result = cv::Mat(mask.size(), CV_8UC3, cv::Scalar(0, 0, 0));
    if (onlyTumor) {
        for (int y = 0; y < mask.rows; ++y)
            for (int x = 0; x < mask.cols; ++x)
                if (mask.at<uchar>(y, x) > 0)
                    result.at<cv::Vec3b>(y, x) = {255, 255, 255};
    }
    cv::resize(result, result, cv::Size(300, 300));
    return QImage(result.data, result.cols, result.rows, result.step, QImage::Format_RGB888).copy();
}

QImage renderThresholded(const cv::Mat& slice) {
    cv::Mat thresholded = applyThreshold(slice);
    cv::resize(thresholded, thresholded, cv::Size(300, 300));
    return QImage(thresholded.data, thresholded.cols, thresholded.rows, thresholded.step, QImage::Format_Grayscale8).copy();
}
