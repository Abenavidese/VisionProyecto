#ifndef SLICEPAGE_H
#define SLICEPAGE_H

#include <QMainWindow>
#include <QLabel>
#include <QSlider>
#include <opencv2/opencv.hpp>
#include <vector>

class SlicePage : public QMainWindow {
    Q_OBJECT

public:
    explicit SlicePage(const std::vector<cv::Mat>& slices,
                       const std::vector<cv::Mat>& masks,
                       QWidget *parent = nullptr);
    ~SlicePage();

private slots:
    void updateSlice(int z); // Actualizar el corte mostrado
    void toggleMask(bool show); // Mostrar/ocultar máscara
    void toggleTumorOnly(bool show); // Mostrar solo el tumor

private:
    // Etiquetas para mostrar las imágenes
    QLabel *originalLabel;   // Imagen original
    QLabel *overlayLabel;    // Tumor resaltado en rojo
    QLabel *tumorOnlyLabel;  // Solo tumor
    QLabel *filteredLabel;   // Resultado del filtro

    QSlider *sliceSlider;    // Control deslizante para navegar entre cortes
    std::vector<cv::Mat> slices; // Cortes de la imagen
    std::vector<cv::Mat> masks;  // Máscaras correspondientes
    int currentZ;            // Índice del corte actual
    bool showMask;           // Estado del checkbox "Mostrar Máscara"
    bool showTumorOnly;      // Estado del checkbox "Solo Tumor"
    bool applyThresholdFilter; // Estado del checkbox "Aplicar Umbralización"

    void displaySlice();     // Mostrar el corte actual en las etiquetas
    cv::Mat applyThreshold(const cv::Mat& inputImage); // Aplicar filtro de umbralización
};

#endif // SLICEPAGE_H
