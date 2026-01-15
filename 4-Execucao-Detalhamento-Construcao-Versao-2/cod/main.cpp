#include "src/View/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Cria a janela principal
    MainWindow w;

    // Mostra a janela na tela
    w.show();

    // Entra no loop de eventos (espera você clicar nos botões)
    return a.exec();
}
