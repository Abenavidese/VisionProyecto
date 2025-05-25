#include "mainwindow.h"
#include "nifti_loader/nifti_loader.h"
#include "slice_extractor/slice_extractor.h"
#include "frontend/welcomepage.h"
#include "frontend/slicepage.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QString>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    welcomePage = new WelcomePage(this);
    QDir().mkpath("output/out_filtro");
    QDir().mkpath("output/out_tumor");
    QDir().mkpath("output/out_video");

    connect(welcomePage, &WelcomePage::nextClicked, this, [this]() {
        welcomePage->close();

        // Llamar a diálogo para cargar imágenes y máscaras
        if (!loadImagesAndMasksInteractive()) {
            QMessageBox::warning(this, tr("Error"), tr("No se pudieron cargar las imágenes o máscaras."));
            return;
        }

        // Crear SlicePage vacío y luego asignar datos
        slicePage = new SlicePage(this);
        slicePage->setSlicesAndMasks(slices, masks);
        slicePage->show();
    });

    welcomePage->show();
}

MainWindow::~MainWindow() {}

bool MainWindow::loadImagesAndMasksInteractive() {
    QString imgPath = QFileDialog::getOpenFileName(this,
                                                   tr("Seleccionar imagen NIfTI"),
                                                   "",
                                                   tr("Archivos NIfTI (*.nii *.nii.gz)"));
    if (imgPath.isEmpty()) {
        QMessageBox::warning(this, tr("Aviso"), tr("No seleccionó archivo de imagen."));
        return false;
    }

    QString maskPath = QFileDialog::getOpenFileName(this,
                                                    tr("Seleccionar máscara NIfTI"),
                                                    "",
                                                    tr("Archivos NIfTI (*.nii *.nii.gz)"));
    if (maskPath.isEmpty()) {
        QMessageBox::warning(this, tr("Aviso"), tr("No seleccionó archivo de máscara."));
        return false;
    }

    auto image = loadImage(imgPath.toStdString());
    if (!image) {
        QMessageBox::critical(this, tr("Error"), tr("No se pudo cargar la imagen."));
        return false;
    }

    auto mask = loadMask(maskPath.toStdString());
    if (!mask) {
        QMessageBox::critical(this, tr("Error"), tr("No se pudo cargar la máscara."));
        return false;
    }

    int totalZ = image->GetLargestPossibleRegion().GetSize()[2];
    slices.clear();
    masks.clear();
    slices.reserve(totalZ);
    masks.reserve(totalZ);

    for (int z = 0; z < totalZ; ++z) {
        slices.push_back(getSlice(image, z));
        masks.push_back(getMaskSlice(mask, z));
    }

    return true;
}
