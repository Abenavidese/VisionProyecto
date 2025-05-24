#ifndef SLICE_RENDERER_H
#define SLICE_RENDERER_H

#include <opencv2/opencv.hpp>
#include <QImage>

QImage renderOriginal(const cv::Mat& slice);
QImage renderOverlay(const cv::Mat& slice, const cv::Mat& mask, bool showMask);
QImage renderTumorOnly(const cv::Mat& mask, bool onlyTumor);
QImage renderThresholded(const cv::Mat& slice);

#endif // SLICE_RENDERER_H
