#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include <opencv2/opencv.hpp>
#include "frontend/welcomepage.h"
#include "frontend/slicepage.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool loadImagesAndMasksInteractive();  // ← corregido

private:
    WelcomePage *welcomePage;  // ← asegurarse de usar este en el .cpp
    SlicePage *slicePage;

    std::vector<cv::Mat> slices;
    std::vector<cv::Mat> masks;
};

#endif // MAINWINDOW_H
