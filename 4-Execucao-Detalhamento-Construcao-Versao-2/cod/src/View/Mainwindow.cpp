#include "Mainwindow.h"
#include "ui_Mainwindow.h"

// Includes dos modelos (Model)
#include "src/Model/CCalculadoraFluxoFracionario.h"
#include "src/Model/ICurvasPermeabilidade.h"
#include "src/Model/CCurvasPermeabilidadeCorey.h"


#include <memory> // Para std::unique_ptr

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Configura o visual inicial do gráfico
    configurarGrafico();

    // Sincroniza o ComboBox com o StackedWidget (Muda a tela quando troca a opção)
    // Nota: Certifique-se que a ordem no ComboBox é a mesma das páginas no QtDesigner
    connect(ui->cbModeloPerm, SIGNAL(currentIndexChanged(int)),
            ui->stkModelos, SLOT(setCurrentIndex(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::configurarGrafico()
{
    // Habilita zoom e arrastar
    ui->plotGrafico->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    // Legendas
    ui->plotGrafico->xAxis->setLabel("Saturação de Água (Sw)");
    ui->plotGrafico->yAxis->setLabel("Fluxo Fracionário (fw)");

    // Adiciona a curva principal (Azul)
    ui->plotGrafico->addGraph();
    ui->plotGrafico->graph(0)->setPen(QPen(Qt::blue));
    ui->plotGrafico->graph(0)->setName("Fluxo Fracionário");

    // Define limites iniciais (0 a 1)
    ui->plotGrafico->xAxis->setRange(0, 1);
    ui->plotGrafico->yAxis->setRange(0, 1);
    ui->plotGrafico->legend->setVisible(true);
}

// ATENÇÃO: O nome deste método deve bater com o nome do botão no .ui (btnPlotarFluido)
void MainWindow::on_btnPlotarFluido_clicked()
{
    // 1. Limpar log
    ui->txtLog->clear();
    ui->txtLog->appendPlainText("Iniciando simulação...");

    // 2. Coletar dados Globais (Viscosidade)
    // Como são QLineEdit, usamos text().toDouble()
    double mu_o = ui->leViscOleo->text().toDouble();
    double mu_w = ui->leViscAgua->text().toDouble();

    // Validação básica
    if (mu_o <= 0 || mu_w <= 0) {
        ui->txtLog->appendPlainText("ERRO: Viscosidades devem ser maiores que zero.");
        return;
    }

    // 3. Instanciar o Modelo de Permeabilidade Correto
    ICurvasPermeabilidade* modeloPermeabilidade = nullptr;
    int index = ui->cbModeloPerm->currentIndex();

    try {
        if (index == 0) { // Modelo Corey
            // Coletando dados dos LineEdits específicos do Corey
            double no = ui->leCoreyNo->text().toDouble();
            double nw = ui->leCoreyNw->text().toDouble();
            double swi = ui->leCoreySwi->text().toDouble();
            double sor = ui->leCoreySor->text().toDouble();
            double krwm = ui->leCoreyKrwm->text().toDouble();
            double krom = ui->leCoreyKrom->text().toDouble();

            // Criando o objeto Corey
            modeloPermeabilidade = new CCurvasPermeabilidadeCorey(krom, krwm, no, nw, swi, sor);
            ui->txtLog->appendPlainText("Modelo Corey selecionado.");
        }
        else if (index == 1) { // Modelo LET (Ainda em desenvolvimento na interface)
            ui->txtLog->appendPlainText("ERRO: Modelo LET ainda não implementado na tela.");
            return;
        }
        else if (index == 2) { // Modelo Chierici
            // Precisaria criar os LineEdits para Chierici no .ui primeiro
            ui->txtLog->appendPlainText("ERRO: Modelo Chierici ainda não implementado na tela.");
            return;
        }

        // 4. Calcular a Curva (Usando a Calculadora)
        if (modeloPermeabilidade) {
            CCalculadoraFluxoFracionario calculadora(mu_o, mu_w, modeloPermeabilidade);

            // Gera a curva (passo de 0.01)
            std::map<double, double> curva = calculadora.gerarCurvaCompleta(0.01);

            // 5. Preparar dados para Plotagem
            QVector<double> x, y;
            for (auto const& [sw, fw] : curva) {
                x.push_back(sw);
                y.push_back(fw);
            }

            // 6. Atualizar Gráfico
            ui->plotGrafico->graph(0)->setData(x, y);
            ui->plotGrafico->rescaleAxes();
            ui->plotGrafico->replot();

            ui->txtLog->appendPlainText("Gráfico gerado com sucesso!");

            // Limpeza de memória
            delete modeloPermeabilidade;
        }

    } catch (std::exception& e) {
        ui->txtLog->appendPlainText("ERRO DE EXCEÇÃO: " + QString(e.what()));
    }
}
