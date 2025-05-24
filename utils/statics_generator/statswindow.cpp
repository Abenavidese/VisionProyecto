#include "statswindow.h"
#include "statics_generator/statistics.h"
#include <QVBoxLayout>
#include <QDebug>

StatsWindow::StatsWindow(const QString& title, int width, int height, QWidget* parent)
    : QWidget(parent)
{
    setWindowTitle(title);
    resize(width, height);

    tabWidget = new QTabWidget(this);

    histPlot = new QCustomPlot(this);
    boxPlot = new QCustomPlot(this);
    statsText = new QTextEdit(this);
    statsText->setReadOnly(true);
    statsText->setFont(QFont("Courier", 10));

    tabWidget->addTab(histPlot, "Histograma");
    tabWidget->addTab(boxPlot, "Boxplot");
    tabWidget->addTab(statsText, "Estadísticas");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(tabWidget);
    setLayout(layout);

    // Forzar inicialización
    histPlot->xAxis->setVisible(true);
    histPlot->yAxis->setVisible(true);
    boxPlot->xAxis->setVisible(true);
    boxPlot->yAxis->setVisible(true);
    histPlot->replot();
    boxPlot->replot();
}

void StatsWindow::updateStats(const cv::Mat& image, const cv::Mat& mask) {
    if (image.empty()) {
        qWarning() << "Imagen vacía.";
        return;
    }

    // Convertir a escala de grises
    cv::Mat gray;
    if (image.channels() > 1)
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    else
        gray = image;

    // Limpiar gráficos y texto
    histPlot->clearGraphs(); histPlot->clearItems();
    boxPlot->clearGraphs(); boxPlot->clearItems();
    statsText->clear();

    // Graficar histogramas y boxplot
    Statistics::plotHistogram(gray, histPlot);
    Statistics::plotBoxplot(gray, boxPlot, mask);

    // Mostrar estadísticas numéricas
    auto stats = Statistics::computeBasicStats(gray, mask);
    QString text = QString("Media: %1\nDesv.E: %2\nMin: %3\nMax: %4\nÁrea: %5")
                       .arg(stats["mean"], 0, 'f', 2)
                       .arg(stats["stddev"], 0, 'f', 2)
                       .arg(stats["min"], 0, 'f', 2)
                       .arg(stats["max"], 0, 'f', 2)
                       .arg(stats["area"], 0, 'f', 2);
    statsText->setText(text);
}
