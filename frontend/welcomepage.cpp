#include "welcomepage.h"

WelcomePage::WelcomePage(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Bienvenido");
    resize(400, 200);

    nextButton = new QPushButton("Siguiente", this);
    nextButton->setGeometry(150, 100, 100, 40);

    connect(nextButton, &QPushButton::clicked, this, [this]() {
        emit nextClicked(); // Emitir señal cuando se presiona "Siguiente"
        close();            // Cerrar la ventana de bienvenida
    });
}
