#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "src/View/qcustomplot.h"
#include "../Model/csimulador.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnSimular_clicked(); // <--- Essa é a assinatura do botão

private:
    Ui::MainWindow *ui;
    CSimulador m_simulador; // A janela agora "possui" um simulador
};
#endif // MAINWINDOW_H
