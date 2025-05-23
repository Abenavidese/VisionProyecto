#include "welcomepage.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFont>

WelcomePage::WelcomePage(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Bienvenido");
    resize(400, 250);

    // Crear layout vertical
    QVBoxLayout* layout = new QVBoxLayout(this);

    // Título principal
    QLabel* titleLabel = new QLabel("Proyecto Integrador", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    // Subtítulo o descripción
    QLabel* subtitleLabel = new QLabel("Visión Artificial - Bryan & Anthony", this);
    QFont subtitleFont = subtitleLabel->font();
    subtitleFont.setPointSize(12);
    subtitleLabel->setFont(subtitleFont);
    subtitleLabel->setAlignment(Qt::AlignCenter);

    // Botón
    nextButton = new QPushButton("Siguiente", this);
    nextButton->setFixedHeight(40);
    nextButton->setFixedWidth(120);
    nextButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    nextButton->setCursor(Qt::PointingHandCursor);

    // Añadir widgets al layout con separación
    layout->addWidget(titleLabel);
    layout->addWidget(subtitleLabel);
    layout->addStretch();  // Empuja el botón hacia abajo
    layout->addWidget(nextButton, 0, Qt::AlignCenter);

    connect(nextButton, &QPushButton::clicked, this, [this]() {
        emit nextClicked();
        close();
    });
}
