#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
// Removemos "csimulador.h" pois ele era numerico
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnSimular_clicked(); // Vamos reescrever a logica dele
    void on_btnLimpar_clicked();  // Limpa o grafico

private:
    Ui::MainWindow *ui;

    // Configura o visual do grafico (eixos, legendas)
    void configurarGrafico();
};
#endif // MAINWINDOW_H
