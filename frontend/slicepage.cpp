#include "slicepage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include <QDir>
#include <numeric>
#include <QCoreApplication>
#include <opencv2/opencv.hpp>
#include <QDirIterator>
#include <QImageReader>
#include "filter_thresholding/threshold.h"
#include "nifti_utils/nifti_utils.h"
#include "slice_renderer/slice_renderer.h"
#include "statics_generator/statswindow.h"
#include "statics_generator/statistics.h"
#include "filter_contrast/contrast.h"
#include "filter_bordes/bordes.h"
#include "filter_color_thresh/color_thresh.h"
#include "filter_pixel_ops/pixel_ops.h"
#include "filter_suavizado/suavizado.h"
#include "filter_morfologia/morfologia.h"
#include "filter_bitwise/bitwise.h"

SlicePage::SlicePage(QWidget *parent)
    : QMainWindow(parent),
    currentZ(0),
    showMask(true),
    showTumorOnly(false),
    applyThresholdFilter(false)
{
    setWindowTitle("Visor de Resonancia");
    resize(1200, 800);


    // Menú: Otras funciones
    QMenu* menuFunciones = menuBar()->addMenu("Otras funciones");
    QAction* actionGenerarVideo = new QAction("🎥 Generar video", this);
    menuFunciones->addAction(actionGenerarVideo);
    connect(actionGenerarVideo, &QAction::triggered, this, &SlicePage::onGenerarVideoClicked);

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
    filterComboBox->addItem("Contrast Stretching");
    filterComboBox->addItem("Detección de Bordes");
    filterComboBox->addItem("Binarización por Color");
    filterComboBox->addItem("Inversión de Intensidad");
    filterComboBox->addItem("Suavizado Gaussiano");
    filterComboBox->addItem("Dilatación");

    // Bitwise directamente aquí
    filterComboBox->addItem("Bitwise NOT");
    filterComboBox->addItem("Bitwise AND");
    filterComboBox->addItem("Bitwise OR");
    filterComboBox->addItem("Bitwise XOR");

    filterComboBox->setEnabled(false);


    loadButton = new QPushButton("📂 Cargar imagen", this);

    // Botones para estadísticas
    QPushButton* tumorStatsButton = new QPushButton("📊 Estadísticas del Tumor", this);
    QPushButton* filterStatsButton = new QPushButton("📊 Estadísticas del Filtro", this);

    QPushButton* saveTumorButton = new QPushButton("💾 Guardar Tumor", this);
    QPushButton* saveFilterButton = new QPushButton("💾 Guardar Filtro", this);


    QPushButton* analizarTumorButton = new QPushButton("📂 Analizar carpeta Tumor", this);
    QPushButton* analizarFiltroButton = new QPushButton("📂 Analizar carpeta Filtro", this);


    // Labels para mostrar imágenes
    originalLabel = new QLabel(this);
    originalLabel->setAlignment(Qt::AlignCenter);
    originalLabel->setStyleSheet("border: 1px solid black;");
    originalLabel->setScaledContents(true);

    overlayLabel = new QLabel(this);
    overlayLabel->setAlignment(Qt::AlignCenter);
    overlayLabel->setStyleSheet("border: 1px solid black;");
    overlayLabel->setScaledContents(true);

    tumorOnlyLabel = new QLabel(this);
    tumorOnlyLabel->setAlignment(Qt::AlignCenter);
    tumorOnlyLabel->setStyleSheet("border: 1px solid black;");
    tumorOnlyLabel->setScaledContents(true);

    filteredLabel = new QLabel(this);
    filteredLabel->setAlignment(Qt::AlignCenter);
    filteredLabel->setStyleSheet("border: 1px solid black;");
    filteredLabel->setScaledContents(true);


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
    controlsLayout->addWidget(saveTumorButton);
    controlsLayout->addWidget(saveFilterButton);
    controlsLayout->addWidget(analizarTumorButton);
    controlsLayout->addWidget(analizarFiltroButton);

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
    connect(saveTumorButton, &QPushButton::clicked, this, &SlicePage::onSaveTumorClicked);
    connect(saveFilterButton, &QPushButton::clicked, this, &SlicePage::onSaveFilterClicked);

    connect(analizarTumorButton, &QPushButton::clicked, this, [this]() {
        analizarCarpeta("output/out_tumor", "tumor");
    });

    connect(analizarFiltroButton, &QPushButton::clicked, this, [this]() {
        analizarCarpeta("output/out_filtro", "filtro");
    });


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
    } else if (selectedFilter == "Contrast Stretching") {
        cv::Mat stretched = aplicarContrastStretching(img);
        QImage out(stretched.data, stretched.cols, stretched.rows, stretched.step, QImage::Format_Grayscale8);
        filteredLabel->setPixmap(QPixmap::fromImage(out.copy()));
    }
    else if (selectedFilter == "Detección de Bordes") {
        cv::Mat bordes = aplicarDeteccionBordes(img);
        QImage out(bordes.data, bordes.cols, bordes.rows, bordes.step, QImage::Format_Grayscale8);
        filteredLabel->setPixmap(QPixmap::fromImage(out.copy()));
    }
    else if (selectedFilter == "Binarización por Color") {
        cv::Mat binarizada = aplicarColorThreshold(img, 100, 200);  // puedes hacer estos valores dinámicos luego
        QImage out(binarizada.data, binarizada.cols, binarizada.rows, binarizada.step, QImage::Format_Grayscale8);
        filteredLabel->setPixmap(QPixmap::fromImage(out.copy()));
    }
    else if (selectedFilter == "Inversión de Intensidad") {
        cv::Mat invertida = aplicarInversion(img);
        QImage out(invertida.data, invertida.cols, invertida.rows, invertida.step, QImage::Format_Grayscale8);
        filteredLabel->setPixmap(QPixmap::fromImage(out.copy()));
    }
    else if (selectedFilter == "Suavizado Gaussiano") {
        cv::Mat suavizada = aplicarSuavizadoGaussiano(img);
        QImage out(suavizada.data, suavizada.cols, suavizada.rows, suavizada.step, QImage::Format_Grayscale8);
        filteredLabel->setPixmap(QPixmap::fromImage(out.copy()));
    }
    else if (selectedFilter == "Dilatación") {
        cv::Mat dilatada = aplicarDilatacion(masks[currentZ]);
        QImage out(dilatada.data, dilatada.cols, dilatada.rows, dilatada.step, QImage::Format_Grayscale8);
        filteredLabel->setPixmap(QPixmap::fromImage(out.copy()));
    }
    else if (selectedFilter == "Bitwise NOT") {
        cv::Mat resultado = applyBitwiseNOT(img);
        QImage out(resultado.data, resultado.cols, resultado.rows, resultado.step, QImage::Format_Grayscale8);
        filteredLabel->setPixmap(QPixmap::fromImage(out.copy()));

    } else if (selectedFilter == "Bitwise AND") {
        cv::Mat resultado = applyBitwiseAND(img, mask);
        QImage out(resultado.data, resultado.cols, resultado.rows, resultado.step, QImage::Format_Grayscale8);
        filteredLabel->setPixmap(QPixmap::fromImage(out.copy()));

    } else if (selectedFilter == "Bitwise OR") {
        cv::Mat resultado = applyBitwiseOR(img, mask);
        QImage out(resultado.data, resultado.cols, resultado.rows, resultado.step, QImage::Format_Grayscale8);
        filteredLabel->setPixmap(QPixmap::fromImage(out.copy()));

    } else if (selectedFilter == "Bitwise XOR") {
        cv::Mat resultado = applyBitwiseXOR(img, mask);
        QImage out(resultado.data, resultado.cols, resultado.rows, resultado.step, QImage::Format_Grayscale8);
        filteredLabel->setPixmap(QPixmap::fromImage(out.copy()));
    }


    else {
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

void SlicePage::onGenerarVideoClicked() {
    if (slices.empty() || masks.empty()) {
        QMessageBox::warning(this, "Advertencia", "No hay datos cargados para generar videos.");
        return;
    }

    // Crear directorio de salida
    QDir baseDir(QCoreApplication::applicationDirPath());
    baseDir.cdUp();  // build/
    baseDir.cdUp();  // VisorNiftiQt/
    QString outputDir = baseDir.filePath("output/out_video");
    QDir().mkpath(outputDir);

    QString pathOriginal = outputDir + "/stack_original.avi";
    QString pathOverlay  = outputDir + "/stack_overlay.avi";
    QString pathFiltered = outputDir + "/stack_filtered.avi";

    cv::Size frameSize(300, 300);
    int fps = 10;

    cv::VideoWriter writerOriginal(pathOriginal.toStdString(),
                                   cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
                                   fps, frameSize, true);

    cv::VideoWriter writerOverlay(pathOverlay.toStdString(),
                                  cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
                                  fps, frameSize, true);

    cv::VideoWriter writerFiltered;
    bool writerFilteredReady = false;

    QString selectedFilter = filterComboBox->currentText();
    bool filterSelected = (selectedFilter != "-- Elegir filtro --");

    if (!writerOriginal.isOpened() || !writerOverlay.isOpened()) {
        QMessageBox::critical(this, "Error", "No se pudo crear los archivos de video originales.");
        return;
    }

    for (size_t i = 0; i < slices.size(); ++i) {
        // Imagen original
        cv::Mat imgGray, imgBGR;
        cv::resize(slices[i], imgGray, frameSize);
        cv::cvtColor(imgGray, imgBGR, cv::COLOR_GRAY2BGR);
        writerOriginal.write(imgBGR);

        // Overlay con máscara
        cv::Mat overlay;
        cv::cvtColor(imgGray, overlay, cv::COLOR_GRAY2BGR);
        for (int y = 0; y < overlay.rows; ++y) {
            for (int x = 0; x < overlay.cols; ++x) {
                if (masks[i].at<uchar>(y * masks[i].rows / overlay.rows,
                                       x * masks[i].cols / overlay.cols) > 0) {
                    overlay.at<cv::Vec3b>(y, x) = {0, 0, 255};
                }
            }
        }
        writerOverlay.write(overlay);

        // Filtro seleccionado
        if (filterSelected) {
            cv::Mat filtered;

            if (selectedFilter == "Umbralización") {
                filtered = applyThreshold(slices[i]);
            } else if (selectedFilter == "Contrast Stretching") {
                filtered = aplicarContrastStretching(slices[i]);
            } else if (selectedFilter == "Detección de Bordes") {
                filtered = aplicarDeteccionBordes(slices[i]);
            }
            else if (selectedFilter == "Binarización por Color") {
                filtered = aplicarColorThreshold(slices[i]);
            }
            else if (selectedFilter == "Inversión de Intensidad") {
                filtered = aplicarInversion(slices[i]);
            }

            else if (selectedFilter == "Suavizado Gaussiano") {
                filtered = aplicarSuavizadoGaussiano(slices[i]);
            }
            else if (selectedFilter == "Dilatación") {
                filtered = aplicarDilatacion(masks[i]);
            }
            else if (selectedFilter == "Bitwise NOT") {
                filtered = applyBitwiseNOT(slices[i]);
            }
            else if (selectedFilter == "Bitwise AND") {
                filtered = applyBitwiseAND(slices[i], masks[i]);
            }
            else if (selectedFilter == "Bitwise OR") {
                filtered = applyBitwiseOR(slices[i], masks[i]);
            }
            else if (selectedFilter == "Bitwise XOR") {
                filtered = applyBitwiseXOR(slices[i], masks[i]);
            }


            else {
                continue;  // Ignorar filtros no implementados aún
            }

            // Abrir writerFiltered solo una vez
            if (!writerFilteredReady) {
                writerFiltered.open(pathFiltered.toStdString(),
                                    cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
                                    fps, frameSize, true);
                if (!writerFiltered.isOpened()) {
                    QMessageBox::critical(this, "Error", "No se pudo crear el archivo del video filtrado.");
                    return;
                }
                writerFilteredReady = true;
            }

            cv::Mat resized, color;
            cv::resize(filtered, resized, frameSize);
            cv::cvtColor(resized, color, cv::COLOR_GRAY2BGR);
            writerFiltered.write(color);
        }
    }

    writerOriginal.release();
    writerOverlay.release();
    if (writerFilteredReady) writerFiltered.release();

    QMessageBox::information(this, "Videos generados", "Se han generado los siguientes videos:\n"
                                                       "- Imagen original\n"
                                                       "- Imagen con máscara\n"
                                                       "- Imagen filtrada (si aplica)\n\nGuardados en:\n" + outputDir);
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

    cv::Mat filtered;

    if (selectedFilter == "Umbralización") {
        filtered = applyThreshold(slices[currentZ]);
    } else if (selectedFilter == "Contrast Stretching") {
        filtered = aplicarContrastStretching(slices[currentZ]);
    } else if (selectedFilter == "Detección de Bordes") {
        filtered = aplicarDeteccionBordes(slices[currentZ]);

    }
    else if (selectedFilter == "Binarización por Color") {
        filtered = aplicarColorThreshold(slices[currentZ]);
    }
    else if (selectedFilter == "Suavizado Gaussiano") {
        filtered = aplicarSuavizadoGaussiano(slices[currentZ]);
    }
    else if (selectedFilter == "Dilatación") {
        filtered = aplicarDilatacion(masks[currentZ]);
    }

    else if (selectedFilter == "NOT") {
        filtered = applyBitwiseNOT(slices[currentZ]);

    } else if (selectedFilter == "AND") {
        filtered = applyBitwiseAND(slices[currentZ], masks[currentZ]);

    } else if (selectedFilter == "OR") {
        filtered = applyBitwiseOR(slices[currentZ], masks[currentZ]);

    } else if (selectedFilter == "XOR") {
        filtered = applyBitwiseXOR(slices[currentZ], masks[currentZ]);
    }

    else {
        QMessageBox::warning(this, "Advertencia", "Selecciona un filtro válido antes de continuar.");
        return;
    }

    StatsWindow* filterStatsWindow = new StatsWindow("Estadísticas del Filtro", 800, 600);
    filterStatsWindow->setAttribute(Qt::WA_DeleteOnClose);  // Se libera al cerrar
    filterStatsWindow->updateStats(filtered, masks[currentZ]);
    filterStatsWindow->show();  // Ventana independiente
}


void SlicePage::onSaveTumorClicked() {
    if (masks.empty()) return;

    QImage tumorImg = renderTumorOnly(masks[currentZ], true);

    // Subir desde /build/Desktop_Qt_*/ al directorio raíz del proyecto
    QDir baseDir(QCoreApplication::applicationDirPath());
    baseDir.cdUp();  // build/
    baseDir.cdUp();  // VisorNiftiQt/

    QString outputDir = baseDir.filePath("output/out_tumor");
    QDir().mkpath(outputDir);

    QString path = QString("%1/tumor_z%2.png").arg(outputDir).arg(currentZ);

    if (tumorImg.save(path)) {
        qDebug() << "✅ Imagen de tumor guardada en:" << path;
    } else {
        QMessageBox::warning(this, "Error", "No se pudo guardar la imagen del tumor.");
    }
}
void SlicePage::onSaveFilterClicked() {
    if (!filteredLabel || filteredLabel->pixmap().isNull()) {
        QMessageBox::warning(this, "Advertencia", "No hay imagen filtrada visible.");
        return;
    }

    QImage filteredImg = filteredLabel->pixmap().toImage();

    QDir baseDir(QCoreApplication::applicationDirPath());
    baseDir.cdUp();  // build/
    baseDir.cdUp();  // VisorNiftiQt/

    QString outputDir = baseDir.filePath("output/out_filtro");
    QDir().mkpath(outputDir);

    QString path = QString("%1/filtro_z%2_t%3.png")
                       .arg(outputDir)
                       .arg(currentZ)
                       .arg(QDateTime::currentSecsSinceEpoch());

    if (filteredImg.save(path)) {
        qDebug() << "✅ Imagen filtrada guardada desde label en:" << path;
    } else {
        QMessageBox::warning(this, "Error", "No se pudo guardar la imagen filtrada.");
    }
}



void SlicePage::analizarCarpeta(const QString& relativePath, const QString& tipo) {
    // Acceder a la ruta del proyecto
    QDir baseDir(QCoreApplication::applicationDirPath());
    baseDir.cdUp();
    baseDir.cdUp();
    QString carpetaCompleta = baseDir.filePath(relativePath);

    if (!QDir(carpetaCompleta).exists()) {
        QMessageBox::warning(this, "Carpeta no encontrada", "No existe la carpeta: " + carpetaCompleta);
        return;
    }

    QDirIterator it(carpetaCompleta, QStringList() << "*.png" << "*.jpg", QDir::Files);
    while (it.hasNext()) {
        QString imagePath = it.next();

        QImage qimg(imagePath);
        if (qimg.isNull()) continue;

        // Convertir a Mat para compatibilidad
        cv::Mat img = Statistics::matFromQImage(qimg);

        // Mostrar ventana de estadísticas
        QString titulo = QString("Estadísticas - %1").arg(QFileInfo(imagePath).fileName());
        StatsWindow* w = new StatsWindow(titulo, 800, 600);
        w->setAttribute(Qt::WA_DeleteOnClose);
        w->updateStats(img);  // sin máscara
        w->show();
    }
}
