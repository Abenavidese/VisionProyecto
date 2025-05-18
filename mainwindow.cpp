#include "mainwindow.h"
#include "nifti_loader/nifti_loader.h"
#include "slice_extractor/slice_extractor.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    loadImagesAndMasks();

    // Mostrar la ventana de bienvenida
    WelcomePage *welcome = new WelcomePage(this);
    connect(welcome, &WelcomePage::nextClicked, this, [this]() {
        SlicePage *slicePage = new SlicePage(slices, masks, this);
        slicePage->show();
    });

    welcome->show();
}

MainWindow::~MainWindow() {}

void MainWindow::loadImagesAndMasks() {
    std::string pathImg = "/home/ubuntu/Desktop/ExamenVision/Dataset/Paciente1/BraTS20_Training_001_t1ce.nii";
    std::string pathMask = "/home/ubuntu/Desktop/ExamenVision/Dataset/Paciente1/BraTS20_Training_001_seg.nii";

    auto image = loadImage(pathImg);
    auto mask = loadMask(pathMask);
    int totalZ = image->GetLargestPossibleRegion().GetSize()[2];

    for (int z = 0; z < totalZ; ++z) {
        slices.push_back(getSlice(image, z));
        masks.push_back(getMaskSlice(mask, z));
    }
}
