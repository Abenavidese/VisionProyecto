#ifndef SLICEPAGE_H
#define SLICEPAGE_H

#include <QMainWindow>
#include <QLabel>
#include <QSlider>
#include <QCheckBox>
#include <QPushButton>
#include <opencv2/opencv.hpp>
#include <vector>

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
    void on_loadButton_clicked(); // Slot para botón "Cargar imagen"

private:
    QLabel *originalLabel;
    QLabel *overlayLabel;
    QLabel *tumorOnlyLabel;
    QLabel *filteredLabel;

    QSlider *sliceSlider;
    QCheckBox *maskCheckBox;
    QCheckBox *tumorOnlyCheckBox;
    QCheckBox *thresholdCheckBox;
    QPushButton *loadButton;

    std::vector<cv::Mat> slices;
    std::vector<cv::Mat> masks;

    int currentZ;
    bool showMask;
    bool showTumorOnly;
    bool applyThresholdFilter;

    void displaySlice();
    cv::Mat applyThreshold(const cv::Mat& inputImage);

    void clearImages();        // Limpia etiquetas
    void enableControls();     // Activa sliders y checkboxes
    void disableControls();    // Desactiva controles si no hay datos

    bool loadImagesAndMasksInteractive();  // Nueva carga desde QFileDialog
};

#endif // SLICEPAGE_H
