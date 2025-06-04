#ifndef SLICEPAGE_H
#define SLICEPAGE_H

#include <QMainWindow>
#include <QLabel>
#include <QSlider>
#include <QCheckBox>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <vector>
#include <opencv2/opencv.hpp>

#include "filter_thresholding/threshold.h"
#include "statics_generator/qcustomplot.h"

class SlicePage : public QMainWindow {
    Q_OBJECT

public:
    explicit SlicePage(QWidget *parent = nullptr);
    ~SlicePage();

    void setSlicesAndMasks(const std::vector<cv::Mat>& newSlices, const std::vector<cv::Mat>& newMasks);

private slots:
    void updateSlice(int z);
    void toggleMask(bool show);
    void toggleTumorOnly(bool show);
    void on_loadButton_clicked();
    void onTumorStatsButtonClicked();
    void onFilterStatsButtonClicked();
    void onGenerarVideoClicked();  // ✅ Añadido

private:
    // Labels para mostrar imágenes
    QLabel *originalLabel;
    QLabel *overlayLabel;
    QLabel *tumorOnlyLabel;
    QLabel *filteredLabel;

    // Layout principal
    QVBoxLayout* mainLayout;         // ✅ Añadido como atributo

    // Controles de usuario
    QSlider *sliceSlider;
    QCheckBox *maskCheckBox;
    QCheckBox *tumorOnlyCheckBox;
    QPushButton *loadButton;

    // Filtros
    QComboBox *filterComboBox;       // ✅ Promovido a atributo
    QComboBox *bitwiseComboBox;      // ✅ Promovido a atributo

    // Botones para estadísticas
    QPushButton *tumorStatsButton;
    QPushButton *filterStatsButton;

    // Gráficos de estadísticas (opcional)
    QCustomPlot *tumorStatsPlot;
    QCustomPlot *filterStatsPlot;

    // Datos
    std::vector<cv::Mat> slices;
    std::vector<cv::Mat> masks;
    int currentZ;
    bool showMask;
    bool showTumorOnly;
    bool applyThresholdFilter;

    // Métodos internos
    void displaySlice();
    void clearImages();
    void enableControls();
    void disableControls();
    void onSaveTumorClicked();
    void onSaveFilterClicked();
    void analizarCarpeta(const QString& relativePath, const QString& tipo);
    void analizarCarpetaYMostrarResumen(const QString& relativePath, const QString& tipo);

    bool loadImagesAndMasksInteractive();
};

#endif // SLICEPAGE_H
