#ifndef WELCOMEPAGE_H
#define WELCOMEPAGE_H

#include <QDialog>
#include <QPushButton>

class WelcomePage : public QDialog {
    Q_OBJECT

public:
    explicit WelcomePage(QWidget *parent = nullptr);

signals:
    void nextClicked(); // Señal para abrir la siguiente ventana

private:
    QPushButton *nextButton;
};

#endif // WELCOMEPAGE_H
