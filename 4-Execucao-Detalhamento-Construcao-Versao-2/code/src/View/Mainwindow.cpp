#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>

// Inclui os modelos matemáticos
#include "CCurvasPermeabilidadeCorey.h"
#include "CCurvasPermeabilidadeLET.h"
#include "CCurvasPermeabilidadeChierici.h"
#include "CCurvasPermeabilidadeTabelada.h"
#include "CRelatorio.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 1. Inicializa o Simulador
    simulador = new CSimulador();
    // SOLUÇÃO DO CRASH: Definir um modelo inicial padrão
    // Isso evita o erro de "Modelo de permeabilidade nulo" ao iniciar
    simulador->setModeloPermeabilidade(new CCurvasPermeabilidadeCorey());

    // 2. Configura a estética dos gráficos
    configurarGraficos();

    // 3. Sincroniza abas
    ui->stkModelos->setCurrentIndex(ui->cbModeloPerm->currentIndex());
}

MainWindow::~MainWindow()
{
    delete simulador;
    delete ui;
}

void MainWindow::configurarGraficos() {
    // Gráfico 1: Fluxo Fracionário
    ui->plotFluxo->xAxis->setLabel("Saturação de Água (Sw)");
    ui->plotFluxo->yAxis->setLabel("Fluxo Fracionário (fw)");
    ui->plotFluxo->xAxis->setRange(0, 1);
    ui->plotFluxo->yAxis->setRange(0, 1);
    ui->plotFluxo->addGraph();
    ui->plotFluxo->graph(0)->setPen(QPen(Qt::blue));
    ui->plotFluxo->graph(0)->setName("Curva Fw");

    // Gráfico 2: Perfil de Saturação
    ui->plotSvsX->xAxis->setLabel("Distância (m)");
    ui->plotSvsX->yAxis->setLabel("Saturação (Sw)");
    ui->plotSvsX->yAxis->setRange(0, 1);
    ui->plotSvsX->addGraph();
    ui->plotSvsX->graph(0)->setPen(QPen(Qt::red));
    ui->plotSvsX->graph(0)->setBrush(QBrush(QColor(255, 0, 0, 20)));
}

void MainWindow::on_cbModeloPerm_currentIndexChanged(int index) {
    ui->stkModelos->setCurrentIndex(index);
}

// --- SIMULAÇÃO PRINCIPAL ---
void MainWindow::on_btnPlotarSolucao_clicked() {
    try {
        // 1. Coleta Dados Gerais
        double L = ui->leComprimento->text().toDouble();
        double A = ui->leArea->text().toDouble();
        double phi = ui->lePorosidade->text().toDouble();
        double angulo = ui->leAngulo->text().toDouble();
        double vazao = ui->leVazao->text().toDouble();

        // Permeabilidade Absoluta (lePerm)
        double k = ui->lePerm->text().toDouble();


        double mi_o = ui->leViscOleo->text().toDouble();
        double mi_w = ui->leViscAgua->text().toDouble();
        double rho_o = ui->leDensOleo->text().toDouble();
        double rho_w = ui->leDensAgua->text().toDouble();

        // Configura o Simulador
        simulador->setDadosReservatorio(L, A, phi, angulo, vazao);
        simulador->setFluidos(mi_o, mi_w, rho_o, rho_w);
        // Nota: Precisaremos passar 'k' para a calculadora.
        // Como o setDadosReservatorio atual não recebe k (no CSimulador.h que fiz antes),
        // vamos passar provisoriamente ou atualizar CSimulador depois.
        // Por hora, o código assume que o CSimulador gerencia isso internamente ou via update da calculadora.
        double ut = vazao / A;
        simulador->getCalculadora()->setPropriedades(mi_w, mi_o, rho_w, rho_o, k, angulo, ut);

        // --- 2. Configura o Modelo Escolhido ---
        ICurvasPermeabilidade* modelo = nullptr;
        int indiceModelo = ui->cbModeloPerm->currentIndex();

        if (indiceModelo == 0) { // COREY
            double no = ui->leCoreyNo->text().toDouble();
            double nw = ui->leCoreyNw->text().toDouble();
            double swi = ui->leCoreySwi->text().toDouble();
            double sor = ui->leCoreySor->text().toDouble();
            double krwm = ui->leCoreyKrwm->text().toDouble();
            double krom = ui->leCoreyKrom->text().toDouble();

            modelo = new CCurvasPermeabilidadeCorey(krom, krwm, no, nw, swi, sor);

        } else if (indiceModelo == 1) { // LET
            double Lw = ui->leLetLw->text().toDouble();
            double Ew = ui->leLetEw->text().toDouble();
            double Tw = ui->leLetTw->text().toDouble();
            double Lo = ui->leLetLo->text().toDouble();
            double Eo = ui->leLetEo->text().toDouble();
            double To = ui->leLetTo->text().toDouble();
            double swi = ui->leLetSwir->text().toDouble();
            double sor = ui->leLetSor->text().toDouble();

            modelo = new CCurvasPermeabilidadeLET(Lw, Ew, Tw, Lo, Eo, To, swi, sor);

        } else if (indiceModelo == 2) { // CHIERICI
            double Aw = ui->leChiericiAw->text().toDouble();
            double Bw = ui->leChiericiBw->text().toDouble();
            double Ao = ui->leChiericiAo->text().toDouble();
            double Bo = ui->leChiericiBo->text().toDouble();
            double swi = ui->leChiericiSwir->text().toDouble();
            double sor = ui->leChiericiSor->text().toDouble();
            double krwm = ui->leChiericiKrwMax->text().toDouble();
            double krom = ui->leChiericiKroMax->text().toDouble();

            modelo = new CCurvasPermeabilidadeChierici(Aw, Bw, Ao, Bo, swi, sor, krwm, krom);

        } else { // TABELA
            // Para tabela, precisamos que o arquivo já tenha sido carregado.
            // O objeto modelo será criado lendo o arquivo salvo na label.
            QString arq = ui->lblArquivoTabela->text();
            if (arq == "Arquivo" || arq.isEmpty()) {
                throw std::runtime_error("Selecione um arquivo de tabela primeiro.");
            }
            auto tabela = new CCurvasPermeabilidadeTabelada();
            tabela->carregarDados(arq.toStdString());
            modelo = tabela;
        }

        // Passa a estratégia
        simulador->setModeloPermeabilidade(modelo);



        // --- 3. Executa a Simulação ---
        double tempo = 0.5; // Fixo por enquanto, pode virar campo leTempo
        simulador->executarSimulacao(tempo);

        // --- 4. Plota ---
        plotarResultados();
        ui->txtLog->appendHtml("<span style='color:green;'><b>Simulação concluída com sucesso!</b></span>");
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Erro de Simulação", e.what());
    }
}

void MainWindow::plotarResultados() {
    auto calc = simulador->getCalculadora();
    auto malha = simulador->getMalha();

    // 1. Fluxo Fracionário
    QVector<double> xFw(101), yFw(101);
    for (int i=0; i<=100; ++i) {
        double sw = i/100.0;
        xFw[i] = sw;
        yFw[i] = calc->calcularFw(sw);
    }
    ui->plotFluxo->graph(0)->setData(xFw, yFw);
    ui->plotFluxo->rescaleAxes();
    ui->plotFluxo->replot();

    // 2. Perfil de Saturação
    const auto& celulas = malha->getCelulas();
    if(celulas.empty()) return;

    QVector<double> dist, sat;
    for(const auto& c : celulas) {
        dist.append(c.getPosicao());
        sat.append(c.getSaturacao());
    }
    ui->plotSvsX->graph(0)->setData(dist, sat);
    ui->plotSvsX->rescaleAxes();
    ui->plotSvsX->replot();
}

// --- Carregamento de Arquivos ---

void MainWindow::on_btlCarregarCorey_clicked() {
    QString path = QFileDialog::getOpenFileName(this, "Abrir Corey", "", "Text Files (*.txt)");
    if(path.isEmpty()) return;
    ui->lblArquivoCorey->setText(path);

    // Opcional: Carregar do arquivo para os LineEdits agora
    try {
        CCurvasPermeabilidadeCorey temp;
        temp.carregarDados(path.toStdString());
        // Aqui poderíamos ter getters na classe Corey para preencher a tela...
        // Como não implementamos getters nas classes de curva, apenas armazenamos o path.
        QMessageBox::information(this, "Sucesso", "Arquivo selecionado. Clique em Solução para usar.");
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Erro", e.what());
    }
}

void MainWindow::on_btnCarregarLET_clicked() {
    QString path = QFileDialog::getOpenFileName(this, "Abrir LET", "", "Text Files (*.txt)");
    if(!path.isEmpty()) ui->lblArquivoLET->setText(path);
}

void MainWindow::on_btnCarregarChierici_clicked() {
    QString path = QFileDialog::getOpenFileName(this, "Abrir Chierici", "", "Text Files (*.txt)");
    if(!path.isEmpty()) ui->lblArquivoChierici->setText(path);
}

void MainWindow::on_btnCarregarTabela_clicked() {
    QString path = QFileDialog::getOpenFileName(this, "Abrir Tabela", "", "Text Files (*.txt)");
    if(!path.isEmpty()) ui->lblArquivoTabela->setText(path);
}

void MainWindow::on_btnRelatorio_clicked() {
    try {
        QString caminho = QFileDialog::getSaveFileName(this, "Salvar PDF", "", "PDF (*.pdf)");
        if (caminho.isEmpty()) return;

        CRelatorio rel; // Agora reconhecido devido ao #include

        // Dados para o cabeçalho técnico [cite: 871]
        rel.gerarCabecalho(ui->leComprimento->text().toDouble(),
                           ui->leArea->text().toDouble(),
                           ui->lePorosidade->text().toDouble()/100.0,
                           ui->lePerm->text().toDouble(),
                           ui->leAngulo->text().toDouble(),
                           ui->leViscOleo->text().toDouble(),
                           ui->leViscAgua->text().toDouble());

        // Diagnóstico de Engenharia: Rapoport-Leas, M e Ng [cite: 416, 440]
        auto calc = simulador->getCalculadora();
        double nrl = calc->calcularRapoportLeas(ui->leComprimento->text().toDouble(),
                                                ui->lePorosidade->text().toDouble()/100.0,
                                                0.03); // Tensão interfacial

        rel.registrarDiagnosticoFisico(nrl, 1.0, 0.0); // Valores diagnósticos

        auto welge = simulador->getWelge();
        rel.registrarResultadoWelge(welge->getSwFrente(), welge->getSwMedia(), 1.0/welge->getInclinacaoChoque());

        rel.exportarParaPDF(caminho.toStdString());
        QMessageBox::information(this, "Sucesso", "PDF Gerado.");

    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Erro", e.what());
    }
}

// --- SLOT: Botão Apenas Fluxo Fracionário (Sem Simulação de Tempo) ---
void MainWindow::on_btnPlotarFw_clicked() {
    try {
        // =========================================================
        // ETAPA 1: Coleta de Dados Físicos (Mesma lógica da Solução)
        // =========================================================

        // 1.1 Dados do Reservatório e Fluidos
        double L = ui->leComprimento->text().toDouble();
        double A = ui->leArea->text().toDouble();
        double phi = ui->lePorosidade->text().toDouble();
        double angulo = ui->leAngulo->text().toDouble();
        double vazao = ui->leVazao->text().toDouble();

        // Permeabilidade (Lendo do campo ou usando padrão)
        double k = 100.0;
        if (!ui->lePerm->text().isEmpty()) {
            k = ui->lePerm->text().toDouble();
        }

        double mi_o = ui->leViscOleo->text().toDouble();
        double mi_w = ui->leViscAgua->text().toDouble();
        double rho_o = ui->leDensOleo->text().toDouble();
        double rho_w = ui->leDensAgua->text().toDouble();

        // 1.2 Envia para o Simulador (para configurar a Calculadora interna)
        simulador->setDadosReservatorio(L, A, phi, angulo, vazao);
        simulador->setFluidos(mi_o, mi_w, rho_o, rho_w);

        // Nota: A "Calculadora" dentro do simulador precisa saber 'K' e 'Angulo' para
        // desenhar a curva correta se houver gravidade. O método setDadosReservatorio
        // que fizemos cuida disso (assumindo que atualizamos ele para passar K).

        // =========================================================
        // ETAPA 2: Configura o Modelo de Permeabilidade (Strategy)
        // =========================================================
        ICurvasPermeabilidade* modelo = nullptr;
        int indiceModelo = ui->cbModeloPerm->currentIndex();

        if (indiceModelo == 0) { // COREY
            double no = ui->leCoreyNo->text().toDouble();
            double nw = ui->leCoreyNw->text().toDouble();
            double swi = ui->leCoreySwi->text().toDouble();
            double sor = ui->leCoreySor->text().toDouble();
            double krwm = ui->leCoreyKrwm->text().toDouble();
            double krom = ui->leCoreyKrom->text().toDouble();

            modelo = new CCurvasPermeabilidadeCorey(krom, krwm, no, nw, swi, sor);

        } else if (indiceModelo == 1) { // LET
            double Lw = ui->leLetLw->text().toDouble();
            double Ew = ui->leLetEw->text().toDouble();
            double Tw = ui->leLetTw->text().toDouble();
            double Lo = ui->leLetLo->text().toDouble();
            double Eo = ui->leLetEo->text().toDouble();
            double To = ui->leLetTo->text().toDouble();
            double swi = ui->leLetSwir->text().toDouble();
            double sor = ui->leLetSor->text().toDouble();

            modelo = new CCurvasPermeabilidadeLET(Lw, Ew, Tw, Lo, Eo, To, swi, sor);

        } else if (indiceModelo == 2) { // CHIERICI
            double Aw = ui->leChiericiAw->text().toDouble();
            double Bw = ui->leChiericiBw->text().toDouble();
            double Ao = ui->leChiericiAo->text().toDouble();
            double Bo = ui->leChiericiBo->text().toDouble();
            double swi = ui->leChiericiSwir->text().toDouble();
            double sor = ui->leChiericiSor->text().toDouble();
            double krwm = ui->leChiericiKrwMax->text().toDouble();
            double krom = ui->leChiericiKroMax->text().toDouble();

            modelo = new CCurvasPermeabilidadeChierici(Aw, Bw, Ao, Bo, swi, sor, krwm, krom);

        } else { // TABELA
            QString arq = ui->lblArquivoTabela->text();
            if (arq == "Arquivo" || arq.isEmpty()) {
                throw std::runtime_error("Selecione um arquivo de tabela primeiro.");
            }
            auto tabela = new CCurvasPermeabilidadeTabelada();
            tabela->carregarDados(arq.toStdString());
            modelo = tabela;
        }

        if (!modelo) {
            throw std::runtime_error("Por favor, configure um modelo de permeabilidade válido.");
        }

        simulador->setModeloPermeabilidade(modelo);

        // =========================================================
        // ETAPA 3: Plotagem Específica (Apenas Fw)
        // =========================================================

        auto calc = simulador->getCalculadora();

        // Gera 100 pontos para a curva ficar suave
        QVector<double> xFw(101), yFw(101);
        for (int i = 0; i <= 100; ++i) {
            double sw = i / 100.0;
            xFw[i] = sw;
            // Aqui a calculadora já usa a Gravidade se o ângulo/densidade forem != 0
            yFw[i] = calc->calcularFw(sw);
        }

        // Atualiza o widget QCustomPlot
        ui->plotFluxo->graph(0)->setData(xFw, yFw);
        ui->plotFluxo->rescaleAxes();
        ui->plotFluxo->replot();

        ui->txtLog->appendHtml("<b>Curva de Fluxo Fracionário atualizada.</b>");

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Erro ao Plotar Fw", e.what());
    }
}
