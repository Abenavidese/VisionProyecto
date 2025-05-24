#ifndef SLICEPAGE_H
#define SLICEPAGE_H

#include <QMainWindow>
#include <QLabel>
#include <QSlider>
#include <QCheckBox>
#include <QPushButton>
#include <QComboBox>
#include <vector>
#include <opencv2/opencv.hpp>

#include "filter_thresholding/threshold.h"
#include "statics_generator/qcustomplot.h"

class SlicePage : public QMainWindow {
    Q_OBJECT

public:
    explicit SlicePage(QWidget *parent = nullptr);
    ~SlicePage();

    /**
     * @brief Establece las imágenes y máscaras cargadas en el visor.
     * @param newSlices Imágenes cargadas como slices.
     * @param newMasks Máscaras cargadas como slices.
     */
    void setSlicesAndMasks(const std::vector<cv::Mat>& newSlices, const std::vector<cv::Mat>& newMasks);

private slots:
    /**
     * @brief Actualiza la vista cuando cambia el slider de slices.
     * @param z Índice del slice seleccionado.
     */
    void updateSlice(int z);

    /**
     * @brief Alterna la visualización de la máscara.
     * @param show Indica si se debe mostrar la máscara.
     */
    void toggleMask(bool show);

    /**
     * @brief Alterna la visualización del tumor únicamente.
     * @param show Indica si se debe mostrar solo el tumor.
     */
    void toggleTumorOnly(bool show);

    /**
     * @brief Maneja el evento de clic en el botón de cargar imágenes.
     */
    void on_loadButton_clicked();

    /**
     * @brief Abre una ventana con las estadísticas del tumor.
     */
    void onTumorStatsButtonClicked();

    /**
     * @brief Abre una ventana con las estadísticas del filtro aplicado.
     */
    void onFilterStatsButtonClicked();

private:
    // Labels para mostrar imágenes
    QLabel *originalLabel;
    QLabel *overlayLabel;
    QLabel *tumorOnlyLabel;
    QLabel *filteredLabel;

    // Controles de usuario
    QSlider *sliceSlider;
    QCheckBox *maskCheckBox;
    QCheckBox *tumorOnlyCheckBox;
    QComboBox *filterComboBox;
    QPushButton *loadButton;

    // Botones para estadísticas
    QPushButton *tumorStatsButton;
    QPushButton *filterStatsButton;

    // Gráficos de estadísticas (opcional, si decides mantenerlos en la misma ventana)
    QCustomPlot *tumorStatsPlot;
    QCustomPlot *filterStatsPlot;

    // Datos
    std::vector<cv::Mat> slices;  // Imágenes cargadas como slices
    std::vector<cv::Mat> masks;   // Máscaras cargadas como slices
    int currentZ;                 // Índice del slice actual
    bool showMask;                // Indica si se muestra la máscara
    bool showTumorOnly;           // Indica si se muestra solo el tumor
    bool applyThresholdFilter;    // Indica si se aplica un filtro de umbralización

    // Métodos internos
    void displaySlice();          // Muestra las imágenes en los labels
    void clearImages();           // Limpia las imágenes mostradas
    void enableControls();        // Habilita los controles de usuario
    void disableControls();       // Deshabilita los controles de usuario
    void onSaveTumorClicked();     // ✅ Agrega esta línea
    void onSaveFilterClicked();    // ✅ Y esta también
    bool loadImagesAndMasksInteractive(); // Carga imágenes y máscaras interactivamente
};

#endif // SLICEPAGE_H
