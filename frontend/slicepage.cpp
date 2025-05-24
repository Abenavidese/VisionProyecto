#include "slicepage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include <opencv2/opencv.hpp>
#include "filter_thresholding/threshold.h"
#include "nifti_utils/nifti_utils.h"
#include "slice_renderer/slice_renderer.h"
#include "statics_generator/statswindow.h"

SlicePage::SlicePage(QWidget *parent)
    : QMainWindow(parent),
    currentZ(0),
    showMask(true),
    showTumorOnly(false),
    applyThresholdFilter(false)
{
    setWindowTitle("Visor de Resonancia");
    resize(1200, 800);

    // Controles de usuario
    sliceSlider = new QSlider(Qt::Horizontal, this);
    sliceSlider->setEnabled(false);

    maskCheckBox = new QCheckBox("Mostrar Máscara", this);
    maskCheckBox->setChecked(showMask);
    maskCheckBox->setEnabled(false);

    tumorOnlyCheckBox = new QCheckBox("Solo Tumor", this);
    tumorOnlyCheckBox->setChecked(showTumorOnly);
    tumorOnlyCheckBox->setEnabled(false);

    filterComboBox = new QComboBox(this);
    filterComboBox->addItem("-- Elegir filtro --");
    filterComboBox->addItem("Umbralización");
    filterComboBox->addItem("CLAHE");
    filterComboBox->addItem("Ecualización");
    filterComboBox->addItem("Gamma");
    filterComboBox->addItem("Sobel");
    filterComboBox->setEnabled(false);

    loadButton = new QPushButton("📂 Cargar imagen", this);

    // Botones para estadísticas
    QPushButton* tumorStatsButton = new QPushButton("📊 Estadísticas del Tumor", this);
    QPushButton* filterStatsButton = new QPushButton("📊 Estadísticas del Filtro", this);

    // Labels para mostrar imágenes
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

    // Layout principal
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    QHBoxLayout* imageLayout = new QHBoxLayout();
    imageLayout->addWidget(originalLabel);
    imageLayout->addWidget(overlayLabel);
    imageLayout->addWidget(tumorOnlyLabel);
    imageLayout->addWidget(filteredLabel);
    mainLayout->addLayout(imageLayout);

    mainLayout->addWidget(sliceSlider);

    QHBoxLayout* controlsLayout = new QHBoxLayout();
    controlsLayout->addWidget(maskCheckBox);
    controlsLayout->addWidget(tumorOnlyCheckBox);
    controlsLayout->addWidget(filterComboBox);
    controlsLayout->addWidget(loadButton);
    controlsLayout->addWidget(tumorStatsButton); // Agregar botón de estadísticas del tumor
    controlsLayout->addWidget(filterStatsButton); // Agregar botón de estadísticas del filtro
    mainLayout->addLayout(controlsLayout);

    setCentralWidget(centralWidget);

    // Conexiones de señales y slots
    connect(sliceSlider, &QSlider::valueChanged, this, &SlicePage::updateSlice);
    connect(maskCheckBox, &QCheckBox::toggled, this, &SlicePage::toggleMask);
    connect(tumorOnlyCheckBox, &QCheckBox::toggled, this, &SlicePage::toggleTumorOnly);
    connect(filterComboBox, &QComboBox::currentIndexChanged, this, [this](int index) {
        displaySlice(); // Actualizar la vista cuando cambia el filtro
    });

    connect(loadButton, &QPushButton::clicked, this, &SlicePage::on_loadButton_clicked);
    connect(tumorStatsButton, &QPushButton::clicked, this, &SlicePage::onTumorStatsButtonClicked);
    connect(filterStatsButton, &QPushButton::clicked, this, &SlicePage::onFilterStatsButtonClicked);
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

    const cv::Mat& img = slices[currentZ];
    const cv::Mat& mask = masks[currentZ];

    originalLabel->setPixmap(QPixmap::fromImage(renderOriginal(img)));
    overlayLabel->setPixmap(QPixmap::fromImage(renderOverlay(img, mask, showMask)));
    tumorOnlyLabel->setPixmap(QPixmap::fromImage(renderTumorOnly(mask, showTumorOnly)));

    QString selectedFilter = filterComboBox->currentText();

    if (selectedFilter == "Umbralización") {
        filteredLabel->setPixmap(QPixmap::fromImage(renderThresholded(img)));
    } else {
        filteredLabel->clear();
    }
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
    filterComboBox->setEnabled(true);
}

void SlicePage::disableControls() {
    sliceSlider->setEnabled(false);
    maskCheckBox->setEnabled(false);
    tumorOnlyCheckBox->setEnabled(false);
    filterComboBox->setEnabled(false);
}

void SlicePage::setSlicesAndMasks(const std::vector<cv::Mat>& newSlices, const std::vector<cv::Mat>& newMasks) {
    slices = newSlices;
    masks = newMasks;
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
        std::vector<cv::Mat> loadedSlices = loadNiftiToCvSlices(imagePath);
        std::vector<cv::Mat> loadedMasks = loadNiftiToCvSlices(maskPath);
        setSlicesAndMasks(loadedSlices, loadedMasks);
        return true;
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString("No se pudieron cargar las imágenes: ") + e.what());
        return false;
    }
}

void SlicePage::onTumorStatsButtonClicked() {
    if (slices.empty() || masks.empty()) {
        QMessageBox::warning(this, "Advertencia", "No hay datos disponibles para mostrar estadísticas.");
        return;
    }

    // Crear ventana emergente (sin this como padre)
    StatsWindow* tumorStatsWindow = new StatsWindow("Estadísticas del Tumor", 800, 600);
    tumorStatsWindow->setAttribute(Qt::WA_DeleteOnClose);  // Se libera al cerrar
    tumorStatsWindow->updateStats(slices[currentZ], masks[currentZ]);
    tumorStatsWindow->show();  // Ventana independiente
}

void SlicePage::onFilterStatsButtonClicked() {
    if (slices.empty() || masks.empty()) {
        QMessageBox::warning(this, "Advertencia", "No hay datos disponibles para mostrar estadísticas.");
        return;
    }

    QString selectedFilter = filterComboBox->currentText();
    if (selectedFilter == "Umbralización") {
        cv::Mat filtered = applyThreshold(slices[currentZ]);

        StatsWindow* filterStatsWindow = new StatsWindow("Estadísticas del Filtro", 800, 600);
        filterStatsWindow->setAttribute(Qt::WA_DeleteOnClose);  // Se libera al cerrar
        filterStatsWindow->updateStats(filtered, masks[currentZ]);
        filterStatsWindow->show();  // Ventana independiente
    } else {
        QMessageBox::warning(this, "Advertencia", "Selecciona un filtro válido antes de continuar.");
    }
}


