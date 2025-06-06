#include "welcomepage.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFont>
#include <QPixmap>
#include <QPalette>
#include <QPushButton>
#include <QSizePolicy>
#include <QMessageBox>

WelcomePage::WelcomePage(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Bienvenido");
    resize(800, 600);

    QString backgroundImagePath = "/home/bryam/Imágenes/Septimo_ciclo/Vision_Computador/PROYECTO/VisionProyecto/logo.jpg";
    QPixmap backgroundPixmap(backgroundImagePath);

    if (!backgroundPixmap.isNull()) {
        setAutoFillBackground(true);
        QPalette palette;
        palette.setBrush(QPalette::Window, QBrush(backgroundPixmap.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
        setPalette(palette);
    } else {
        QPalette palette;
        palette.setColor(QPalette::Window, QColor(255, 255, 255));
        setPalette(palette);
    }

    setAutoFillBackground(true);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 30, 30, 30);

    QLabel* titleLabel = new QLabel("Proyecto Integrador", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(30);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: black;");

    QLabel* subtitleLabel = new QLabel("Visión Artificial - Bryan & Anthony", this);
    QFont subtitleFont = subtitleLabel->font();
    subtitleFont.setPointSize(18);
    subtitleLabel->setFont(subtitleFont);
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet("color: black;");

    nextButton = new QPushButton("Siguiente", this);
    nextButton->setFixedHeight(60);
    nextButton->setFixedWidth(180);
    nextButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    nextButton->setCursor(Qt::PointingHandCursor);
    nextButton->setStyleSheet(
        "background-color: #4CAF50; "
        "color: white; "
        "border-radius: 12px; "
        "box-shadow: 0px 4px 8px rgba(0, 0, 0, 0.2);"
        );

    nextButton->setStyleSheet("QPushButton:hover {background-color: #45a049;}");

    layout->addWidget(titleLabel);
    layout->addWidget(subtitleLabel);
    layout->addStretch();
    layout->addWidget(nextButton, 0, Qt::AlignCenter);

    connect(nextButton, &QPushButton::clicked, this, [this]() {
        emit nextClicked();
        close();
    });
}
