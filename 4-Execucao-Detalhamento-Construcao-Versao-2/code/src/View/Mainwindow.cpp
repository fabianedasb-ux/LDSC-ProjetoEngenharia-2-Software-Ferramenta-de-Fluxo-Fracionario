#include "Mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <algorithm>
static constexpr double SWI_EPS = 1e-3;

// Inclui os modelos matemáticos
#include "CCurvasPermeabilidadeCorey.h"
#include "CCurvasPermeabilidadeLET.h"
#include "CCurvasPermeabilidadeChierici.h"
#include "CCurvasPermeabilidadeTabelada.h"
#include "ICurvasPermeabilidade.h"
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
    // 1. Fluxo Fracionário (Adicionando a reta tangente de Welge)
    ui->plotFluxo->xAxis->setLabel("Saturação de Água (Sw)");
    ui->plotFluxo->yAxis->setLabel("Fluxo Fracionário (fw)");
    ui->plotFluxo->addGraph(); // Gráfico 0: Curva Fw (Azul)
    ui->plotFluxo->graph(0)->setPen(QPen(Qt::blue));
    ui->plotFluxo->addGraph(); // Gráfico 1: Tangente de Welge (Tracejado Vermelho)
    ui->plotFluxo->graph(1)->setPen(QPen(Qt::red, 1, Qt::DashLine));

    // 2. Perfil de Saturação (Agora Sw vs xD)
    ui->plotSvsX->xAxis->setLabel("Posição Adimensional (xD)");
    ui->plotSvsX->yAxis->setLabel("Saturação (Sw)");
    ui->plotSvsX->xAxis->setRange(0, 1.0); // xD máximo físico = 1.0
    ui->plotSvsX->addGraph();
    ui->plotSvsX->graph(0)->setPen(QPen(Qt::red));

    // 3. Eficiência de Deslocamento vs PVI [cite: 627, 631]
    ui->plotEfDesl->xAxis->setLabel("Tempo Adimensional (PVI)");
    ui->plotEfDesl->yAxis->setLabel("Eficiência de Deslocamento (Ed)");
    ui->plotEfDesl->addGraph(); // Curva Ed
    ui->plotEfDesl->addGraph(); // Ponto de Breakthrough (Destaque)
    ui->plotEfDesl->graph(1)->setLineStyle(QCPGraph::lsNone);
    ui->plotEfDesl->graph(1)->setScatterStyle(QCPScatterStyle::ssDisc);
}

void MainWindow::on_cbModeloPerm_currentIndexChanged(int index) {
    ui->stkModelos->setCurrentIndex(index);
}

void MainWindow::sincronizarDadosComSimulador() {
    // 1. Coleta e Conversão de Dados Escalares
    double L     = ui->leComprimento->text().toDouble(); // 100 m
    double A     = ui->leArea->text().toDouble(); // 1 m²
    double phi   = ui->lePorosidade->text().toDouble() / 100.0; // 20 %
    double ang   = ui->leAngulo->text().toDouble(); // 0 graus
    double vazao = ui->leVazao->text().toDouble(); // 1 m³/d
    double k     = ui->lePerm->text().toDouble(); // 100 mD

    double mi_o  = ui->leViscOleo->text().toDouble(); // 1 cP
    double mi_w  = ui->leViscAgua->text().toDouble(); // 1 cP
    double rho_o = ui->leDensOleo->text().toDouble(); // 800 kg/m³
    double rho_w = ui->leDensAgua->text().toDouble(); // 1000 kg/m³

    // 2. Sincroniza Propriedades do Reservatório e Fluidos
    simulador->setDadosReservatorio(L, A, phi, ang, vazao);
    simulador->setFluidos(mi_o, mi_w, rho_o, rho_w);
    simulador->setPermeabilidade(k);  // Define a permeabilidade absoluta lida da UI

    // 3. Atualiza a Calculadora (Crítico para Gravidade e Rapoport-Leas)
    simulador->getCalculadora()->setPropriedades(mi_w, mi_o, rho_w, rho_o, k, ang, vazao, A);
    double ut =  vazao/A;

    // =========================================================
    // ETAPA 2: Configura o Modelo de Permeabilidade (Strategy)
    // =========================================================
    ICurvasPermeabilidade* modelo = nullptr;
    int indiceModelo = ui->cbModeloPerm->currentIndex();

    if (indiceModelo == 0) { // COREY
        double no = ui->leCoreyNo->text().toDouble(); // 2 inteiro
        double nw = ui->leCoreyNw->text().toDouble(); // 2 inteiro
        double swi = ui->leCoreySwi->text().toDouble(); // 0.2 fração
        double sor = ui->leCoreySor->text().toDouble(); // 0.2 fração
        double krwm = ui->leCoreyKrwm->text().toDouble(); // 1.0 fração
        double krom = ui->leCoreyKrom->text().toDouble();  // 1.0 fração

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

}

double MainWindow::obterSaturacaoOleoResidualUI() const {
    // Verifica qual índice (aba) está selecionado no QStackedWidget
    int indiceAtivo = ui->stkModelos->currentIndex();

    switch (indiceAtivo) {
    case 0: // Aba Corey
        return ui->leCoreySor->text().toDouble();
    case 1: // Aba LET
        return ui->leLetSor->text().toDouble();
    case 2: // Aba Chierici
        return ui->leChiericiSor->text().toDouble();
    case 3: // Aba Tabela
        // Em modelos de tabela, Swi geralmente é o primeiro valor da coluna Sw
        // Para simplificar, você pode retornar o valor de uma variável global
        // ou buscar do objeto CCurvasPermeabilidadeTabelada se ele estiver carregado.
        return 0.2; // Valor padrão de segurança ou buscar do objeto tabela
    default:
        return 0.0;
    }
}

double MainWindow::obterSaturacaoInicialUI() const {
    // Verifica qual índice (aba) está selecionado no QStackedWidget
    int indiceAtivo = ui->stkModelos->currentIndex();

    switch (indiceAtivo) {
    case 0: // Aba Corey
        return ui->leCoreySwi->text().toDouble();
    case 1: // Aba LET
        return ui->leLetSwir->text().toDouble();
    case 2: // Aba Chierici
        return ui->leChiericiSwir->text().toDouble();
    case 3: // Aba Tabela
        // Em modelos de tabela, Swi geralmente é o primeiro valor da coluna Sw
        // Para simplificar, você pode retornar o valor de uma variável global
        // ou buscar do objeto CCurvasPermeabilidadeTabelada se ele estiver carregado.
        return 0.2; // Valor padrão de segurança ou buscar do objeto tabela
    default:
        return 0.0;
    }
}
// --- SLOT: Botão Apenas Fluxo Fracionário (Sem Simulação de Tempo) ---
void MainWindow::on_btnPlotarFw_clicked() {
    try {
        sincronizarDadosComSimulador(); // Dados unificados
        auto calc = simulador->getCalculadora();
        // Obter Swi de forma consistente com o modelo selecionado (mesma fonte usada em sincronizarDadosComSimulador)
        double sor = obterSaturacaoOleoResidualUI();
        ui->txtLog->appendHtml("<b>--- Diagnóstico de Engenharia ---</b>");

                // Chamadas diretas conforme implementado na classe
        double sw;
        double vazao;
        double A;
        ui->txtLog->appendHtml(QString("N. Rapoport-Leas: %1").arg(calc->calcularRapoportLeas(simulador->getComprimento(), simulador->getPorosidade(), 0.03, sw, vazao, A ), 0, 'f', 2));
        // Usar a saturação inicial (Swi) para o diagnóstico mostrado na interface
        // (evita apresentar o valor final da curva que pode ser zero).
        double sw_report = 1-sor;
        // Mostrar Krw/Kro em Swi para diagnóstico
        auto modelo = simulador->getModeloPermeabilidade();

        double M_sw = calc->calcularM(sw_report);
        double Ng_sw = calc->calcularNg(sw_report,vazao,A);


        QVector<double> xFw(101), yFw(101);

        for (int i = 0; i <= 100; ++i) {
            double sw = i / 100.0;
            xFw[i] = sw;
            yFw[i] = calc->calcularFw(sw,vazao,A); // Usa a Eq. 3.10 corrigida
        }

        ui->plotFluxo->graph(0)->setData(xFw, yFw);
        ui->plotFluxo->xAxis->setRange(0, 1.0);
        ui->plotFluxo->graph(0)->rescaleAxes();
        // pequena margem visual
        double yMin = ui->plotFluxo->yAxis->range().lower;
        double yMax = ui->plotFluxo->yAxis->range().upper;
        double margin = 0.05 * (yMax - yMin);
        ui->plotFluxo->yAxis->setRange(yMin - margin, yMax + margin);

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Erro", e.what());
    }
}


// --- SIMULAÇÃO PRINCIPAL ---
void MainWindow::on_btnPlotarSolucao_clicked() {
    try {
        sincronizarDadosComSimulador();

        // Lê tempo informado pelo usuário (unidades: horas)
        double t_input = ui->leTempo->text().toDouble();
        if (t_input <= 0.0) {
            throw std::runtime_error("Informe um tempo válido no campo Tempo.");
        }


        double L = ui->leComprimento->text().toDouble();
        double A = ui->leArea->text().toDouble();
        double phi = ui->lePorosidade->text().toDouble() / 100.0;
        double permeabilidade_abs = ui->lePerm->text().toDouble();
        double mi_w  = ui->leViscAgua->text().toDouble();
        double vazao = ui->leVazao->text().toDouble();

        if (A <= 0.0 || phi <= 0.0 || L <= 0.0) {
            throw std::runtime_error("Verifique Comprimento, Área e Porosidade (devem ser > 0).");
        }

        double ct = 8*10^(-5); // compressibilidade total ((kgf/cm²)^-1), valor típico para água e óleo
        double c = 0.0003484;
        double tempo_adimensional = (c * permeabilidade_abs * (t_input)) / (phi * (L*L) * ct * mi_w ); // t_input em horas, convertendo para dias

        // Executa simulação no tempo adimensional calculado
        simulador->executarSimulacao(tempo_adimensional,vazao,A);

        plotarResultados();
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Erro", e.what());
    }
}
// criar plotarFluxoFracionario
// criar plotarPerfildeSaturacao
// criar plotarEfcienciaDeslocamento
void MainWindow::plotarResultados() {
    auto calc = simulador->getCalculadora();
    auto welge = simulador->getWelge();
    auto malha = simulador->getMalha();
    double A = ui->leArea->text().toDouble();
    double vazao = ui->leVazao->text().toDouble();
    const auto& celulas = malha->getCelulas();

    // 1. Atualizar Perfil Sw vs xD
    QVector<double> xD, Sw;
    for(const auto& c : celulas) {
        // xD = x/L. Como no CSolver calculamos xD = vD * tD, usamos direto [cite: 562]
        xD.append(c.getPosicao());
        Sw.append(c.getSaturacao());
    }
    ui->plotSvsX->graph(0)->setData(xD, Sw);

    // Auto-scale para Sw vs xD
    ui->plotSvsX->xAxis->setRange(0, 1.0);
    ui->plotSvsX->graph(0)->rescaleAxes();
    double yMinS = ui->plotSvsX->yAxis->range().lower;
    double yMaxS = ui->plotSvsX->yAxis->range().upper;
    double yMarginS = 0.05 * (yMaxS - yMinS);
    ui->plotSvsX->yAxis->setRange(yMinS - yMarginS, yMaxS + yMarginS);


    // 2. Atualizar Fw com Tangente de Welge (se houver choque) [cite: 609]
    // Desenhar apenas quando a tangente de Welge efetivamente produz choque
    double swi_for_plot = obterSaturacaoInicialUI();
    double swFrente = welge->getSwFrente();
    if (swFrente > swi_for_plot + 1e-12) {
        // usar a inclinação calculada por CWelge e o ponto de contato (swFrente, fw_frente)
        double slope = welge->getInclinacaoChoque();
        double fw_frente = calc->calcularFw(swFrente,vazao,A);
        double sw_media = welge->getSwMedia();

        // reta tangente: y = fw_frente + slope * (x - swFrente)
        double y1 = fw_frente;
        double y2 = fw_frente + slope * (sw_media - swFrente);
        y2 = std::clamp(y2, 0.0, 1.0);

        QVector<double> xTan = {swFrente, sw_media};
        QVector<double> yTan = {y1, y2};
        ui->plotFluxo->graph(1)->setData(xTan, yTan);
        ui->plotFluxo->rescaleAxes();
        double yMinF = ui->plotFluxo->yAxis->range().lower;
        double yMaxF = ui->plotFluxo->yAxis->range().upper;
        double yMarginF = 0.05 * (yMaxF - yMinF);
        ui->plotFluxo->yAxis->setRange(yMinF - yMarginF, yMaxF + yMarginF);
    } else {
        // Nenhum choque — limpar a reta tangente (não desenhar)
        // QCPGraph may not have clearData(); clear underlying data container instead.
        if (ui->plotFluxo->graphCount() > 1 && ui->plotFluxo->graph(1)->data()) {
            ui->plotFluxo->graph(1)->data()->clear();
        }
    }

    // 3. Eficiência de Deslocamento vs PVI [cite: 631, 636]
    double t_bt = 1.0 / welge->getInclinacaoChoque(); // Tempo de Breakthrough
    double swi = obterSaturacaoInicialUI();
    double sw_media = welge->getSwMedia();

    QVector<double> tPVI, Ed;
    for (int i = 0; i <= 100; ++i) {
        double t = i * 0.02; // PVI de 0 a 2
        double Np;
        if (t <= t_bt) {
            Np = t;
        } else {
            Np = (sw_media - swi) + (1.0 - sw_media) * (t - t_bt);
        }
        tPVI.append(t);
        Ed.append(Np / (1.0 - swi)); // Eq. 3.19 adaptada
    }
    ui->plotEfDesl->graph(0)->setData(tPVI, Ed);

    // Destacar o Breakthrough no gráfico
    double ed_bt = t_bt / (1.0 - swi);
    ui->plotEfDesl->graph(1)->setData({t_bt}, {ed_bt});
    ui->plotEfDesl->graph(0)->rescaleAxes();
    ui->plotEfDesl->xAxis->setRange(0, 2.0);
    double yMinE = ui->plotEfDesl->yAxis->range().lower;
    double yMaxE = ui->plotEfDesl->yAxis->range().upper;
    double yMarginE = 0.05 * (yMaxE - yMinE);
    ui->plotEfDesl->yAxis->setRange(yMinE - yMarginE, yMaxE + yMarginE);

    // Replotar todos
    ui->plotFluxo->replot();
    ui->plotSvsX->replot();
    ui->plotEfDesl->replot();
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
        double A = ui->leArea->text().toDouble();
        double vazao = ui->leVazao->text().toDouble();
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
        double sor = obterSaturacaoOleoResidualUI();
        double nrl = calc->calcularRapoportLeas(ui->leComprimento->text().toDouble(),
                                                ui->lePorosidade->text().toDouble()/100.0,
                                                0.03,sor, vazao, A); // Tensão interfacial

        // Calcular M e Ng usando a saturação inicial (Swi) — mesma escolha da interface

        double sw_report = 1-sor;
               double M = calc->calcularM(sw_report);
        double Ng = calc->calcularNg(sw_report, vazao, A);

        double M_to_report = M;
        double Ng_to_report = Ng;


        rel.registrarDiagnosticoFisico(nrl, M_to_report, Ng_to_report);

        auto welge = simulador->getWelge();
        rel.registrarResultadoWelge(welge->getSwFrente(), welge->getSwMedia(), 1.0/welge->getInclinacaoChoque());

        rel.exportarParaPDF(caminho.toStdString());
        QMessageBox::information(this, "Sucesso", "PDF Gerado.");

    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Erro", e.what());
    }
}

