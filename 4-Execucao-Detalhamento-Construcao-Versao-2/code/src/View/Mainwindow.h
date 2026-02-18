#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "CSimulador.h"

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
    // Slot para o botão de Fluxo Fracionário
    void on_btnPlotarFw_clicked();

    // Slot principal de simulação (Solução)
    void on_btnPlotarSolucao_clicked();

    // Slot para relatório
    void on_btnRelatorio_clicked();

    // Troca de abas (Corey <-> LET <-> Chierici)
    void on_cbModeloPerm_currentIndexChanged(int index);

    // Slots para carregar arquivos de cada modelo
    void on_btlCarregarCorey_clicked();
    void on_btnCarregarLET_clicked();
    void on_btnCarregarChierici_clicked();
    void on_btnCarregarTabela_clicked();

private:
    Ui::MainWindow *ui;
    CSimulador *simulador;

    // Métodos auxiliares
    void sincronizarDadosComSimulador();
    void configurarGraficos();
    void plotarResultados();
    double obterSaturacaoOleoResidualUI() const;
    double obterSaturacaoInicialUI() const;
};

#endif // MAINWINDOW_H
