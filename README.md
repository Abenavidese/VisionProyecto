1- CREAR CARPETA DEL FILTRO

-Cick derecho en filter ---> add new
-En add new agregar individualmente el cpp y el h, ya que agregar ambos a la vez no permite crear una nueva carpeta, por ejemplo filter_tr/tr.h , no lo permite, agregando indivualmente si permite

-Agregar el .cpp y el .h

filter_gaussiano/gaussiano.cpp
filter_gaussiano/gaussiano.h

con esto se crea la carpeta gaussiano dentro del filter y se le agrega el header y el soucer file


2- Agregar los metodos

Primero definir el gaussiano.h

Luego definir el gaussiano.cpp

3 - Actualizar el CMAKE

Ir a #fuentes en el CMAKE

Acuatualizar el cmake con la nueva carpeta, guardar el archivo y esperar que compile para verificar que no hay errores

ejemplo

    filters/filter_gaussiano/gaussiano.cpp
    filters/filter_gaussiano/gaussiano.h


4- SLICE PAGE .CPP

INCLUIR EL .H AL INCIO DEL ARCHIVO

#include "filter_contrast/contrast.h"

IR Y AGREGAR EL NUEVO METODO AL COMBO BOX

    filterComboBox = new QComboBox(this);
    filterComboBox->addItem("-- Elegir filtro --");
    filterComboBox->addItem("Umbralización");
    filterComboBox->addItem("Contrast Stretching");

    filterComboBox->setEnabled(false);



IR A VOID DISPLAY SLICE Y ACTUALIZAR EL METODO CON EL NUEVO FILTRO


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
    } else {
        filteredLabel->clear();
    }
}



ACTUALIZAR SELECCIONAR FILTRO


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
    } else {
        QMessageBox::warning(this, "Advertencia", "Selecciona un filtro válido antes de continuar.");
        return;
    }

    StatsWindow* filterStatsWindow = new StatsWindow("Estadísticas del Filtro", 800, 600);
    filterStatsWindow->setAttribute(Qt::WA_DeleteOnClose);  // Se libera al cerrar
    filterStatsWindow->updateStats(filtered, masks[currentZ]);
    filterStatsWindow->show();  // Ventana independiente
}
