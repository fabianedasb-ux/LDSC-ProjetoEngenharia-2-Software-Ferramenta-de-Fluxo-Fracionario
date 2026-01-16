#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QCustomplot.h"

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
    // ATENÇÃO: Mudou de on_btnSimular... para on_btnPlotarFluido...
    void on_btnPlotarFluido_clicked();

private:
    Ui::MainWindow *ui;
    void configurarGrafico();
};
#endif // MAINWINDOW_H
