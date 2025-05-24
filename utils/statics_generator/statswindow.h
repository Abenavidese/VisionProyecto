#ifndef STATS_WINDOW_H
#define STATS_WINDOW_H

#include <QWidget>
#include <QTabWidget>
#include <QTextEdit>
#include "statics_generator/qcustomplot.h"
#include <opencv2/opencv.hpp>

class StatsWindow : public QWidget {
    Q_OBJECT

public:
    explicit StatsWindow(const QString& title, int width = 600, int height = 400, QWidget* parent = nullptr);
    void updateStats(const cv::Mat& image, const cv::Mat& mask = cv::Mat());

private:
    QTabWidget* tabWidget;
    QCustomPlot* histPlot;
    QCustomPlot* boxPlot;
    QTextEdit* statsText;
};

#endif // STATS_WINDOW_H
