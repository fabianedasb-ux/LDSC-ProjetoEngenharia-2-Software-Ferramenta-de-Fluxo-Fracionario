#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // --- CONFIGURAÇÃO DO GRÁFICO ---
    // Cria o gráfico 0 (linha azul)
    ui->plotSaturacao->addGraph();
    ui->plotSaturacao->graph(0)->setPen(QPen(Qt::blue)); // Linha azul
    ui->plotSaturacao->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // Preenchimento leve

    // Nomes dos Eixos
    ui->plotSaturacao->xAxis->setLabel("Distância (m)");
    ui->plotSaturacao->yAxis->setLabel("Saturação de Água (Sw)");

    // Define os limites iniciais (X de 0 a 1000m, Y de 0 a 1.0)
    ui->plotSaturacao->xAxis->setRange(0, 1000);
    ui->plotSaturacao->yAxis->setRange(0, 1.1);

    // Permite zoom e arrastar (opcional, fica chique)
    ui->plotSaturacao->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnSimular_clicked()
{
    // 1. AVISO DE INÍCIO
    ui->txtLog->appendPlainText(">>> Lendo dados da interface...");

    // 2. LER DADOS DA TELA
    double viscOleo = ui->leViscOleo->text().toDouble();
    if (viscOleo == 0) viscOleo = 2.0;

    double viscAgua = ui->leViscAgua->text().toDouble();
    if (viscAgua == 0) viscAgua = 1.0;

    double porosidade = ui->lePorosidade->text().toDouble();
    double tempoTotal = ui->leTempo->text().toDouble();
    if (tempoTotal == 0) tempoTotal = 50.0;

    // 3. CONFIGURAR O SIMULADOR (MODO HD - 500 CÉLULAS)
    ui->txtLog->appendPlainText("Configurando malha (Alta Resolução)...");

    // Configura: Tempo, dx=2.0m, 500 Células, Injeção=1.0
    // Lembrete: 1000 metros / 500 células = 2.0 metros por célula
    m_simulador.setDados(tempoTotal, 2.0, 500, 1.0);

    m_simulador.setPropriedadesFisicas(viscOleo, viscAgua, porosidade);

    // 4. EXECUTAR
    ui->txtLog->appendPlainText("Iniciando Simulação (Newton-Raphson)... aguarde.");

    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_simulador.Executar();
    QApplication::restoreOverrideCursor();

    ui->txtLog->appendPlainText(">>> Simulação Finalizada!");
    ui->txtLog->appendPlainText("------------------------------------------------");

    // --- RECUPERAR DADOS (Fazemos isso UMA vez só) ---
    CMalha* malha = m_simulador.getMalha();
    std::vector<CCelula>& celulas = malha->getCelulas();

    // --- PARTE A: LOG DE TEXTO (Amostragem) ---
    QString linhaResultado = "";
    // Vamos pular de 10 em 10 para não lotar o log (já que são 500 agora)
    for(size_t i = 0; i < celulas.size(); i+=10) {
        double sw = celulas[i].getSaturacao();
        linhaResultado += QString("[%1]: %2  |  ").arg(i).arg(sw, 0, 'f', 4);

        if ((i/10 + 1) % 5 == 0) { // Quebra linha visualmente
            ui->txtLog->appendPlainText(linhaResultado);
            linhaResultado = "";
        }
    }
    if (!linhaResultado.isEmpty()) {
        ui->txtLog->appendPlainText(linhaResultado);
    }

    // --- PARTE B: GRÁFICO HD (Todos os pontos) ---
    int N = 500;
    QVector<double> x(N), y(N);

    for (int i = 0; i < N; ++i)
    {
        x[i] = i * 2.0; // Posição real em metros

        // Correção do Warning: static_cast<size_t> garante comparação segura
        if (static_cast<size_t>(i) < celulas.size()) {
            y[i] = celulas[i].getSaturacao();
        } else {
            y[i] = 0.2; // Segurança
        }
    }

    ui->plotSaturacao->graph(0)->setData(x, y);
    ui->plotSaturacao->replot();

    ui->txtLog->appendPlainText(">>> Gráfico HD Gerado com Sucesso!");
}
