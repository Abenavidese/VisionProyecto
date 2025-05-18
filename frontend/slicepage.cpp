#include "slicepage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include "filter_thresholding/threshold.h"



SlicePage::SlicePage(const std::vector<cv::Mat>& slices_,
                     const std::vector<cv::Mat>& masks_,
                     QWidget *parent)
    : QMainWindow(parent), slices(slices_), masks(masks_), currentZ(0),
    showMask(true), showTumorOnly(false), applyThresholdFilter(false) {

    setWindowTitle("Visor de Resonancia");
    resize(1200, 800); // Aumentamos el tamaño para acomodar varias imágenes

    // Crear widgets
    sliceSlider = new QSlider(Qt::Horizontal, this);
    sliceSlider->setRange(0, slices.size() - 1);
    sliceSlider->setValue(currentZ);

    QCheckBox *maskCheckBox = new QCheckBox("Mostrar Máscara", this);
    maskCheckBox->setChecked(showMask);

    QCheckBox *tumorOnlyCheckBox = new QCheckBox("Solo Tumor", this);
    tumorOnlyCheckBox->setChecked(showTumorOnly);

    QCheckBox *thresholdCheckBox = new QCheckBox("Aplicar Umbralización", this);
    thresholdCheckBox->setChecked(false);

    // Etiquetas para mostrar las imágenes
    QLabel *originalLabel = new QLabel(this);
    originalLabel->setAlignment(Qt::AlignCenter);
    originalLabel->setStyleSheet("border: 1px solid black;");

    QLabel *overlayLabel = new QLabel(this);
    overlayLabel->setAlignment(Qt::AlignCenter);
    overlayLabel->setStyleSheet("border: 1px solid black;");

    QLabel *tumorOnlyLabel = new QLabel(this);
    tumorOnlyLabel->setAlignment(Qt::AlignCenter);
    tumorOnlyLabel->setStyleSheet("border: 1px solid black;");

    QLabel *filteredLabel = new QLabel(this);
    filteredLabel->setAlignment(Qt::AlignCenter);
    filteredLabel->setStyleSheet("border: 1px solid black;");

    // Layout principal
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Layout para las imágenes (4 columnas)
    QHBoxLayout *imageLayout = new QHBoxLayout();
    imageLayout->addWidget(originalLabel);
    imageLayout->addWidget(overlayLabel);
    imageLayout->addWidget(tumorOnlyLabel);
    imageLayout->addWidget(filteredLabel);

    // Agregar widgets al layout principal
    mainLayout->addLayout(imageLayout);
    mainLayout->addWidget(sliceSlider);
    mainLayout->addWidget(maskCheckBox);
    mainLayout->addWidget(tumorOnlyCheckBox);
    mainLayout->addWidget(thresholdCheckBox);

    setCentralWidget(centralWidget);

    // Conexiones
    connect(sliceSlider, &QSlider::valueChanged, this, &SlicePage::updateSlice);
    connect(maskCheckBox, &QCheckBox::toggled, this, &SlicePage::toggleMask);
    connect(tumorOnlyCheckBox, &QCheckBox::toggled, this, &SlicePage::toggleTumorOnly);
    connect(thresholdCheckBox, &QCheckBox::toggled, this, [this](bool checked) {
        applyThresholdFilter = checked;
        displaySlice();
    });

    // Guardar referencias a las etiquetas
    this->originalLabel = originalLabel;
    this->overlayLabel = overlayLabel;
    this->tumorOnlyLabel = tumorOnlyLabel;
    this->filteredLabel = filteredLabel;

    // Mostrar el primer corte
    displaySlice();
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
    cv::Mat img = slices[currentZ].clone();
    cv::Mat maskSlice = masks[currentZ].clone();

    // 1. Imagen original
    cv::Mat resizedImg;
    cv::resize(img, resizedImg, cv::Size(300, 300)); // Redimensionar para ajustar
    QImage qOriginal(resizedImg.data, resizedImg.cols, resizedImg.rows,
                     resizedImg.step, QImage::Format_Grayscale8);
    originalLabel->setPixmap(QPixmap::fromImage(qOriginal));

    // 2. Tumor resaltado en rojo
    cv::Mat overlay;
    cv::cvtColor(img, overlay, cv::COLOR_GRAY2BGR);
    for (int y = 0; y < maskSlice.rows; ++y) {
        for (int x = 0; x < maskSlice.cols; ++x) {
            if (maskSlice.at<uchar>(y, x) > 0) {
                overlay.at<cv::Vec3b>(y, x) = {0, 0, 255}; // Rojo
            }
        }
    }
    cv::resize(overlay, overlay, cv::Size(300, 300));
    QImage qOverlay(overlay.data, overlay.cols, overlay.rows,
                    overlay.step, QImage::Format_RGB888);
    overlayLabel->setPixmap(QPixmap::fromImage(qOverlay));

    // 3. Solo tumor
    cv::Mat tumorOnly = cv::Mat(maskSlice.size(), CV_8UC3, cv::Scalar(0, 0, 0));
    for (int y = 0; y < maskSlice.rows; ++y) {
        for (int x = 0; x < maskSlice.cols; ++x) {
            if (maskSlice.at<uchar>(y, x) > 0) {
                tumorOnly.at<cv::Vec3b>(y, x) = {255, 255, 255}; // Blanco
            }
        }
    }
    cv::resize(tumorOnly, tumorOnly, cv::Size(300, 300));
    QImage qTumorOnly(tumorOnly.data, tumorOnly.cols, tumorOnly.rows,
                      tumorOnly.step, QImage::Format_RGB888);
    tumorOnlyLabel->setPixmap(QPixmap::fromImage(qTumorOnly));

    // 4. Filtro de umbralización (opcional)
    if (applyThresholdFilter) {
        cv::Mat filtered = applyThreshold(img); // Aplica el filtro de umbralización
        cv::resize(filtered, filtered, cv::Size(300, 300));
        QImage qFiltered(filtered.data, filtered.cols, filtered.rows,
                         filtered.step, QImage::Format_Grayscale8);
        filteredLabel->setPixmap(QPixmap::fromImage(qFiltered));
    } else {
        filteredLabel->clear(); // Limpia la etiqueta si no se aplica el filtro
    }
}

// Función para aplicar el filtro de umbralización
cv::Mat SlicePage::applyThreshold(const cv::Mat& inputImage) {
    cv::Mat outputImage;
    cv::threshold(inputImage, outputImage, 128, 255, cv::THRESH_BINARY);
    return outputImage;
}
