#include "statistics.h"
#include <QVector>
#include <QString>
#include <algorithm>
#include <numeric>
#include <QDebug>

namespace Statistics {

// Definición de funciones sin redeclaración explícita

void plotHistogram(const cv::Mat& image, QCustomPlot* plotWidget) {
    if (!plotWidget || image.empty()) {
        qCritical() << "Error: QCustomPlot o imagen no válidos.";
        return;
    }

    // Convertir la imagen a un solo canal si es necesario
    cv::Mat singleChannelImage;
    if (image.channels() > 1) {
        cv::cvtColor(image, singleChannelImage, cv::COLOR_BGR2GRAY);
    } else {
        singleChannelImage = image;
    }

    int histSize = 256;
    float range[] = {0, 256};
    const float* histRange = {range};

    cv::Mat hist;
    cv::calcHist(&singleChannelImage, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange);

    QVector<double> x(histSize), y(histSize);
    for (int i = 0; i < histSize; ++i) {
        x[i] = i;
        y[i] = hist.at<float>(i);
    }

    plotWidget->clearGraphs();
    plotWidget->addGraph();
    plotWidget->graph(0)->setData(x, y);
    plotWidget->xAxis->setLabel("Intensidad");
    plotWidget->yAxis->setLabel("Frecuencia");
    plotWidget->rescaleAxes();
    plotWidget->replot();
}

std::map<std::string, double> computeBasicStats(const cv::Mat& image, const cv::Mat& mask) {
    if (image.empty()) return {};

    cv::Scalar mean, stddev;
    double minVal = 0, maxVal = 0;
    cv::minMaxLoc(image, &minVal, &maxVal, nullptr, nullptr, mask);
    cv::meanStdDev(image, mean, stddev, mask);

    double area = mask.empty() ? image.total() : cv::countNonZero(mask);

    return {
        {"mean", mean[0]},
        {"stddev", stddev[0]},
        {"min", minVal},
        {"max", maxVal},
        {"area", static_cast<double>(area)}
    };
}

void plotBoxplot(const cv::Mat& image, QCustomPlot* plotWidget, const cv::Mat& mask) {
    std::vector<uchar> pixels;

    for (int y = 0; y < image.rows; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        const uchar* mrow = mask.empty() ? nullptr : mask.ptr<uchar>(y);
        for (int x = 0; x < image.cols; ++x) {
            if (mask.empty() || mrow[x] > 0)
                pixels.push_back(row[x]);
        }
    }

    if (pixels.size() < 5) {
        qWarning() << "Muy pocos datos para boxplot.";
        return;
    }

    std::sort(pixels.begin(), pixels.end());

    auto percentile = [&](double p) -> double {
        size_t idx = static_cast<size_t>(p * pixels.size());
        return pixels[std::min(idx, pixels.size() - 1)];
    };

    double Q1 = percentile(0.25);
    double Q2 = percentile(0.50);
    double Q3 = percentile(0.75);
    double IQR = Q3 - Q1;
    double lowerWhisker = Q1 - 1.5 * IQR;
    double upperWhisker = Q3 + 1.5 * IQR;

    // Valores ajustados
    double minVal = *std::find_if(pixels.begin(), pixels.end(), [&](uchar val) { return val >= lowerWhisker; });
    double maxVal = *std::find_if(pixels.rbegin(), pixels.rend(), [&](uchar val) { return val <= upperWhisker; });

    // --- DIBUJAR CAJA Y BIGOTES COMO GRÁFICOS ---

    // Caja entre Q1 y Q3
    QCPGraph* boxGraph = plotWidget->addGraph();
    boxGraph->addData(1, Q1);
    boxGraph->addData(1, Q3);
    boxGraph->setPen(QPen(Qt::blue, 8));  // Caja gruesa

    // Línea de mediana (Q2)
    QCPGraph* medianGraph = plotWidget->addGraph();
    medianGraph->addData(0.8, Q2);
    medianGraph->addData(1.2, Q2);
    medianGraph->setPen(QPen(Qt::red, 2));

    // Bigotes
    QCPGraph* whiskerGraph = plotWidget->addGraph();
    whiskerGraph->addData(1, minVal);
    whiskerGraph->addData(1, Q1);
    whiskerGraph->addData(1, Q3);
    whiskerGraph->addData(1, maxVal);
    whiskerGraph->setLineStyle(QCPGraph::lsLine);
    whiskerGraph->setPen(QPen(Qt::black));

    // Outliers
    QVector<double> xOutliers, yOutliers;
    for (double val : pixels) {
        if (val < lowerWhisker || val > upperWhisker) {
            xOutliers.push_back(1);
            yOutliers.push_back(val);
        }
    }

    QCPGraph* outliers = plotWidget->addGraph();
    outliers->setLineStyle(QCPGraph::lsNone);
    outliers->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::red, Qt::red, 5));
    outliers->setData(xOutliers, yOutliers);

    // Etiquetas y límites
    plotWidget->xAxis->setLabel("Boxplot");
    plotWidget->yAxis->setLabel("Intensidad");
    plotWidget->xAxis->setRange(0.5, 1.5);
    plotWidget->yAxis->setRange(0, 255);
    plotWidget->replot();
}



void plotFullStats(const cv::Mat& image, QCustomPlot* plotWidget, const cv::Mat& mask) {
    if (!plotWidget || image.empty()) {
        qWarning() << "Error: QCustomPlot o imagen no válidos.";
        return;
    }

    // Verificación segura de ejes
    if (!plotWidget->xAxis || !plotWidget->yAxis) {
        qCritical() << "Error: Los ejes de QCustomPlot no están inicializados.";
        return;
    }

    qDebug() << "QCustomPlot y sus ejes están listos para ser usados.";

    // Convertir a escala de grises si es necesario
    cv::Mat singleChannelImage;
    if (image.channels() > 1) {
        cv::cvtColor(image, singleChannelImage, cv::COLOR_BGR2GRAY);
    } else {
        singleChannelImage = image;
    }

    // Limpiar contenido del gráfico
    plotWidget->clearPlottables();
    plotWidget->clearItems();

    // ⚠️ NO borrar todo el layout. Solo remover textos anteriores
    int rowCount = plotWidget->plotLayout()->rowCount();
    for (int i = rowCount - 1; i > 0; --i) {
        QCPLayoutElement* el = plotWidget->plotLayout()->elementAt(i);
        if (el) {
            plotWidget->plotLayout()->removeAt(i);
            delete el;
        }
    }

    // Agregar título
    plotWidget->plotLayout()->insertRow(0);
    plotWidget->plotLayout()->addElement(0, 0,
                                         new QCPTextElement(plotWidget, "Estadísticas completas", QFont("sans", 12, QFont::Bold)));

    // Dibujar histogramas y boxplots
    plotHistogram(singleChannelImage, plotWidget);
    plotBoxplot(singleChannelImage, plotWidget, mask);

    // Calcular estadísticas
    auto stats = computeBasicStats(singleChannelImage, mask);
    QString text = QString("Media: %1\nDesv.E: %2\nMin: %3\nMax: %4\nÁrea: %5")
                       .arg(stats["mean"], 0, 'f', 2)
                       .arg(stats["stddev"], 0, 'f', 2)
                       .arg(stats["min"], 0, 'f', 2)
                       .arg(stats["max"], 0, 'f', 2)
                       .arg(stats["area"], 0, 'f', 2);

    // Texto con estadísticas
    int textRow = plotWidget->plotLayout()->rowCount();
    plotWidget->plotLayout()->insertRow(textRow);
    plotWidget->plotLayout()->addElement(textRow, 0,
                                         new QCPTextElement(plotWidget, text, QFont("Courier", 10)));

    // Mostrar ejes y actualizar
    plotWidget->xAxis->setVisible(true);
    plotWidget->yAxis->setVisible(true);
    plotWidget->rescaleAxes();
    plotWidget->replot();
}


QImage matToQImage(const cv::Mat& mat) {
    if (mat.empty()) return QImage();
    if (mat.channels() == 1)
        return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8).copy();
    else if (mat.channels() == 3) {
        cv::Mat rgb;
        cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
        return QImage(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888).copy();
    }
    return QImage();
}

} // namespace Statistics
