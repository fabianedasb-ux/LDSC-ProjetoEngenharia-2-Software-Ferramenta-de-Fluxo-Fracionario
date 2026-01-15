#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
, ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Configuração inicial do QCustomPlot
    configurarGrafico();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::configurarGrafico()
{
    // Habilita zoom e arrastar
    ui->plotGrafico->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    // Define nomes dos eixos
    ui->plotGrafico->xAxis->setLabel("Saturação de Água (Sw) ou Distância (x)");
    ui->plotGrafico->yAxis->setLabel("Fluxo Fracionário (fw)");

    // Cria um grafico vazio (Graph 0) para plotar depois
    ui->plotGrafico->addGraph();
    ui->plotGrafico->graph(0)->setPen(QPen(Qt::blue)); // Linha azul
    ui->plotGrafico->graph(0)->setName("Curva Analítica");

    // Define o range inicial (0 a 1 para saturação/fluxo)
    ui->plotGrafico->xAxis->setRange(0, 1);
    ui->plotGrafico->yAxis->setRange(0, 1);

    ui->plotGrafico->legend->setVisible(true);
}

void MainWindow::on_btnSimular_clicked()
{
    // AQUI ENTRARÁ A NOVA LÓGICA (ETAPA SEGUINTE)
    // 1. Instanciar ICurvasPermeabilidade (Corey/LET/Chierici)
    // 2. Instanciar CalculadoraFluxoFracionario
    // 3. Pegar dados da tela
    // 4. Calcular e Plotar
}

void MainWindow::on_btnLimpar_clicked()
{
    // Limpa os dados do grafico 0
    ui->plotGrafico->graph(0)->data()->clear();
    ui->plotGrafico->replot();
}
