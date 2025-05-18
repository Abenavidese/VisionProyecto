#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    // Crear la aplicación Qt
    QApplication app(argc, argv);

    // Crear la ventana principal
    MainWindow mainWindow;

    // Mostrar la ventana principal
    mainWindow.show();

    // Ejecutar el bucle de eventos de Qt
    return app.exec();
}
