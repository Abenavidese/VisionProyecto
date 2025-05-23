#include "slicepage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkExtractImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>
#include <opencv2/opencv.hpp>

using ImageType3D = itk::Image<unsigned char, 3>;
using ImageType2D = itk::Image<unsigned char, 2>;

SlicePage::SlicePage(QWidget *parent)
    : QMainWindow(parent),
    currentZ(0),
    showMask(true),
    showTumorOnly(false),
    applyThresholdFilter(false)
{
    setWindowTitle("Visor de Resonancia");
    resize(1200, 800);

    // Crear widgets
    sliceSlider = new QSlider(Qt::Horizontal, this);
    sliceSlider->setEnabled(false);

    maskCheckBox = new QCheckBox("Mostrar Máscara", this);
    maskCheckBox->setChecked(showMask);
    maskCheckBox->setEnabled(false);

    tumorOnlyCheckBox = new QCheckBox("Solo Tumor", this);
    tumorOnlyCheckBox->setChecked(showTumorOnly);
    tumorOnlyCheckBox->setEnabled(false);

    thresholdCheckBox = new QCheckBox("Aplicar Umbralización", this);
    thresholdCheckBox->setChecked(false);
    thresholdCheckBox->setEnabled(false);

    loadButton = new QPushButton("📂 Cargar imagen", this);

    originalLabel = new QLabel(this);
    originalLabel->setAlignment(Qt::AlignCenter);
    originalLabel->setStyleSheet("border: 1px solid black;");

    overlayLabel = new QLabel(this);
    overlayLabel->setAlignment(Qt::AlignCenter);
    overlayLabel->setStyleSheet("border: 1px solid black;");

    tumorOnlyLabel = new QLabel(this);
    tumorOnlyLabel->setAlignment(Qt::AlignCenter);
    tumorOnlyLabel->setStyleSheet("border: 1px solid black;");

    filteredLabel = new QLabel(this);
    filteredLabel->setAlignment(Qt::AlignCenter);
    filteredLabel->setStyleSheet("border: 1px solid black;");

    // Layouts
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QHBoxLayout *imageLayout = new QHBoxLayout();
    imageLayout->addWidget(originalLabel);
    imageLayout->addWidget(overlayLabel);
    imageLayout->addWidget(tumorOnlyLabel);
    imageLayout->addWidget(filteredLabel);
    mainLayout->addLayout(imageLayout);

    mainLayout->addWidget(sliceSlider);

    QHBoxLayout *controlsLayout = new QHBoxLayout();
    controlsLayout->addWidget(maskCheckBox);
    controlsLayout->addWidget(tumorOnlyCheckBox);
    controlsLayout->addWidget(thresholdCheckBox);
    controlsLayout->addWidget(loadButton);
    mainLayout->addLayout(controlsLayout);

    setCentralWidget(centralWidget);

    // Conexiones
    connect(sliceSlider, &QSlider::valueChanged, this, &SlicePage::updateSlice);
    connect(maskCheckBox, &QCheckBox::toggled, this, &SlicePage::toggleMask);
    connect(tumorOnlyCheckBox, &QCheckBox::toggled, this, &SlicePage::toggleTumorOnly);
    connect(thresholdCheckBox, &QCheckBox::toggled, this, [this](bool checked){
        applyThresholdFilter = checked;
        displaySlice();
    });
    connect(loadButton, &QPushButton::clicked, this, &SlicePage::on_loadButton_clicked);
}

SlicePage::~SlicePage() {}

void SlicePage::updateSlice(int z) {
    currentZ = z;
    displaySlice();
}

void SlicePage::toggleMask(bool show) {
    showMask = show;
    displaySlice();
}

void SlicePage::toggleTumorOnly(bool show) {
    showTumorOnly = show;
    displaySlice();
}

void SlicePage::displaySlice() {
    if (slices.empty() || masks.empty()) {
        clearImages();
        return;
    }

    cv::Mat img = slices[currentZ];
    cv::Mat maskSlice = masks[currentZ];

    // Imagen original
    cv::Mat resizedImg;
    cv::resize(img, resizedImg, cv::Size(300,300));
    QImage qOriginal(resizedImg.data, resizedImg.cols, resizedImg.rows, resizedImg.step, QImage::Format_Grayscale8);
    originalLabel->setPixmap(QPixmap::fromImage(qOriginal));

    // Overlay
    cv::Mat overlay;
    cv::cvtColor(img, overlay, cv::COLOR_GRAY2BGR);
    if (showMask) {
        for (int y=0; y < maskSlice.rows; ++y)
            for (int x=0; x < maskSlice.cols; ++x)
                if (maskSlice.at<uchar>(y,x) > 0)
                    overlay.at<cv::Vec3b>(y,x) = {0,0,255};
    }
    cv::resize(overlay, overlay, cv::Size(300,300));
    QImage qOverlay(overlay.data, overlay.cols, overlay.rows, overlay.step, QImage::Format_RGB888);
    overlayLabel->setPixmap(QPixmap::fromImage(qOverlay));

    // Solo tumor
    cv::Mat tumorOnly = cv::Mat(maskSlice.size(), CV_8UC3, cv::Scalar(0,0,0));
    if (showTumorOnly) {
        for (int y=0; y < maskSlice.rows; ++y)
            for (int x=0; x < maskSlice.cols; ++x)
                if (maskSlice.at<uchar>(y,x) > 0)
                    tumorOnly.at<cv::Vec3b>(y,x) = {255,255,255};
    }
    cv::resize(tumorOnly, tumorOnly, cv::Size(300,300));
    QImage qTumorOnly(tumorOnly.data, tumorOnly.cols, tumorOnly.rows, tumorOnly.step, QImage::Format_RGB888);
    tumorOnlyLabel->setPixmap(QPixmap::fromImage(qTumorOnly));

    // Umbral
    if (applyThresholdFilter) {
        cv::Mat filtered = applyThreshold(img);
        cv::resize(filtered, filtered, cv::Size(300,300));
        QImage qFiltered(filtered.data, filtered.cols, filtered.rows, filtered.step, QImage::Format_Grayscale8);
        filteredLabel->setPixmap(QPixmap::fromImage(qFiltered));
    } else {
        filteredLabel->clear();
    }
}

cv::Mat SlicePage::applyThreshold(const cv::Mat& inputImage) {
    cv::Mat outputImage;
    cv::threshold(inputImage, outputImage, 128, 255, cv::THRESH_BINARY);
    return outputImage;
}

void SlicePage::clearImages() {
    originalLabel->clear();
    overlayLabel->clear();
    tumorOnlyLabel->clear();
    filteredLabel->clear();
}

void SlicePage::enableControls() {
    sliceSlider->setEnabled(true);
    maskCheckBox->setEnabled(true);
    tumorOnlyCheckBox->setEnabled(true);
    thresholdCheckBox->setEnabled(true);
}

void SlicePage::disableControls() {
    sliceSlider->setEnabled(false);
    maskCheckBox->setEnabled(false);
    tumorOnlyCheckBox->setEnabled(false);
    thresholdCheckBox->setEnabled(false);
}

void SlicePage::setSlicesAndMasks(const std::vector<cv::Mat>& newSlices, const std::vector<cv::Mat>& newMasks) {
    this->slices = newSlices;
    this->masks = newMasks;
    if (!slices.empty()) {
        sliceSlider->setRange(0, slices.size() - 1);
        sliceSlider->setValue(0);
        currentZ = 0;
        enableControls();
        displaySlice();
    }
}

void SlicePage::on_loadButton_clicked() {
    if (loadImagesAndMasksInteractive()) {
        sliceSlider->setRange(0, slices.size() - 1);
        sliceSlider->setValue(0);
        currentZ = 0;
        enableControls();
        displaySlice();
    }
}

bool SlicePage::loadImagesAndMasksInteractive() {
    QString imagePath = QFileDialog::getOpenFileName(this, "Seleccionar imagen NIfTI", "", "NIfTI (*.nii *.nii.gz)");
    if (imagePath.isEmpty()) return false;

    QString maskPath = QFileDialog::getOpenFileName(this, "Seleccionar máscara NIfTI", "", "NIfTI (*.nii *.nii.gz)");
    if (maskPath.isEmpty()) return false;

    try {
        auto loadNiftiToCvSlices = [](const QString& path) -> std::vector<cv::Mat> {
            using ReaderType = itk::ImageFileReader<ImageType3D>;
            ReaderType::Pointer reader = ReaderType::New();
            reader->SetFileName(path.toStdString());
            reader->Update();
            ImageType3D::Pointer image = reader->GetOutput();

            std::vector<cv::Mat> output;
            ImageType3D::RegionType region = image->GetLargestPossibleRegion();
            int depth = region.GetSize()[2];

            for (int z = 0; z < depth; ++z) {
                ImageType3D::IndexType start = {0, 0, z};
                ImageType3D::SizeType size = {region.GetSize()[0], region.GetSize()[1], 0};
                ImageType3D::RegionType sliceRegion(start, size);

                using ExtractFilterType = itk::ExtractImageFilter<ImageType3D, ImageType2D>;
                ExtractFilterType::Pointer extractor = ExtractFilterType::New();
                extractor->SetExtractionRegion(sliceRegion);
                extractor->SetInput(image);
                extractor->SetDirectionCollapseToIdentity();
                extractor->Update();

                using RescaleType = itk::RescaleIntensityImageFilter<ImageType2D, ImageType2D>;
                RescaleType::Pointer rescaler = RescaleType::New();
                rescaler->SetInput(extractor->GetOutput());
                rescaler->SetOutputMinimum(0);
                rescaler->SetOutputMaximum(255);
                rescaler->Update();

                ImageType2D::Pointer slice = rescaler->GetOutput();
                cv::Mat cvSlice(slice->GetLargestPossibleRegion().GetSize()[1],
                                slice->GetLargestPossibleRegion().GetSize()[0],
                                CV_8UC1,
                                (void*)slice->GetBufferPointer());
                output.push_back(cv::Mat(cvSlice).clone());
            }
            return output;
        };

        std::vector<cv::Mat> loadedSlices = loadNiftiToCvSlices(imagePath);
        std::vector<cv::Mat> loadedMasks = loadNiftiToCvSlices(maskPath);
        setSlicesAndMasks(loadedSlices, loadedMasks);
        return true;

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString("No se pudieron cargar las imágenes: ") + e.what());
        return false;
    }
}
