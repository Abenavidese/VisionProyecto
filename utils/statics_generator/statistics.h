#ifndef STATISTICS_H
#define STATISTICS_H

#include <opencv2/opencv.hpp>
#include "statics_generator/qcustomplot.h"
#include <map>
#include <string>

namespace Statistics {

// Funciones declaradas dentro del namespace
void plotHistogram(const cv::Mat& image, QCustomPlot* plotWidget);
void plotBoxplot(const cv::Mat& image, QCustomPlot* plotWidget, const cv::Mat& mask = cv::Mat());
std::map<std::string, double> computeBasicStats(const cv::Mat& image, const cv::Mat& mask = cv::Mat());
void plotFullStats(const cv::Mat& image, QCustomPlot* plotWidget, const cv::Mat& mask = cv::Mat());
QImage matToQImage(const cv::Mat& mat);

} // namespace Statistics

#endif // STATISTICS_H
