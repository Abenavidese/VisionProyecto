#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "frontend//welcomepage.h"
#include "frontend/slicepage.h"
#include <vector>
#include <opencv2/opencv.hpp>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    std::vector<cv::Mat> slices;
    std::vector<cv::Mat> masks;

    void loadImagesAndMasks();
};

#endif // MAINWINDOW_H
