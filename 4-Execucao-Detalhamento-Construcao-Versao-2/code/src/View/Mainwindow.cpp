#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>

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
    ui->plotSvsX->xAxis->setRange(0, 1.2); // Mostra até um pouco depois do produtor
    ui->plotSvsX->addGraph();
    ui->plotSvsX->graph(0)->setPen(QPen(Qt::red));

    // 3. Velocidade Adimensional vs xD [cite: 532, 560]
    ui->plotVel->xAxis->setLabel("Posição Adimensional (xD)");
    ui->plotVel->yAxis->setLabel("Velocidade Adimensional (vD)");
    ui->plotVel->addGraph();
    ui->plotVel->graph(0)->setPen(QPen(Qt::darkGreen));

    // 4. Eficiência de Deslocamento vs PVI [cite: 627, 631]
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
    double L     = ui->leComprimento->text().toDouble();
    double A     = ui->leArea->text().toDouble();
    double phi   = ui->lePorosidade->text().toDouble() / 100.0; // % -> fração
    double ang   = ui->leAngulo->text().toDouble();
    double vazao = ui->leVazao->text().toDouble();
    double k     = ui->lePerm->text().toDouble(); // Permeabilidade

    double mi_o  = ui->leViscOleo->text().toDouble();
    double mi_w  = ui->leViscAgua->text().toDouble();
    double rho_o = ui->leDensOleo->text().toDouble();
    double rho_w = ui->leDensAgua->text().toDouble();

    // 2. Sincroniza Propriedades do Reservatório e Fluidos
    simulador->setDadosReservatorio(L, A, phi, ang, vazao);
    simulador->setFluidos(mi_o, mi_w, rho_o, rho_w);

    // 3. Atualiza a Calculadora (Crítico para Gravidade e Rapoport-Leas)
    double ut = vazao / A;
    simulador->getCalculadora()->setPropriedades(mi_w, mi_o, rho_w, rho_o, k, ang, ut);

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
        double swi = obterSaturacaoInicialUI();
        ui->txtLog->appendHtml("<b>--- Diagnóstico de Engenharia ---</b>");

        // Chamadas diretas conforme implementado na classe
        ui->txtLog->appendHtml(QString("N. Rapoport-Leas: %1").arg(calc->calcularRapoportLeas(simulador->getComprimento(), simulador->getPorosidade(), 0.03), 0, 'f', 2));
        ui->txtLog->appendHtml(QString("Razão de Mobilidade (M @ Swi): %1").arg(calc->calcularM(swi), 0, 'f', 3));
        ui->txtLog->appendHtml(QString("Número de Gravidade (Ng @ Swi): %1").arg(calc->calcularNg(swi), 0, 'f', 3));


        QVector<double> xFw(101), yFw(101);

        for (int i = 0; i <= 100; ++i) {
            double sw = i / 100.0;
            xFw[i] = sw;
            yFw[i] = calc->calcularFw(sw); // Usa a Eq. 3.10 corrigida
        }

        ui->plotFluxo->graph(0)->setData(xFw, yFw);
        ui->plotFluxo->replot();
        ui->txtLog->appendHtml("Curva Fw atualizada com dados unificados.");

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Erro", e.what());
    }
}


// --- SIMULAÇÃO PRINCIPAL ---
void MainWindow::on_btnPlotarSolucao_clicked() {
    try {
        sincronizarDadosComSimulador();

        // Executa simulação
        simulador->executarSimulacao(0.5);

        plotarResultados();
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Erro", e.what());
    }
}

void MainWindow::plotarResultados() {
    auto calc = simulador->getCalculadora();
    auto welge = simulador->getWelge();
    auto malha = simulador->getMalha();
    const auto& celulas = malha->getCelulas();

    // 1. Atualizar Perfil Sw vs xD e Velocidade vs xD
    QVector<double> xD, Sw, vD;
    for(const auto& c : celulas) {
        // xD = x/L. Como no CSolver calculamos xD = vD * tD, usamos direto [cite: 562]
        xD.append(c.getPosicao());
        Sw.append(c.getSaturacao());
        vD.append(c.getDerivadaFluxo());
    }
    ui->plotSvsX->graph(0)->setData(xD, Sw);
    ui->plotVel->graph(0)->setData(xD, vD);

    // 2. Atualizar Fw com Tangente de Welge (se houver choque) [cite: 609]
    if (welge->getSwFrente() > ui->leCoreySwi->text().toDouble()) {
        QVector<double> xTan = {ui->leCoreySwi->text().toDouble(), welge->getSwMedia()};
        QVector<double> yTan = {calc->calcularFw(ui->leCoreySwi->text().toDouble()), 1.0};
        ui->plotFluxo->graph(1)->setData(xTan, yTan);
    }

    // 3. Eficiência de Deslocamento vs PVI [cite: 631, 636]
    double t_bt = 1.0 / welge->getInclinacaoChoque(); // Tempo de Breakthrough [cite: 635]
    double swi = ui->leCoreySwi->text().toDouble();

    QVector<double> tPVI, Ed;
    for (int i = 0; i <= 100; ++i) {
        double t = i * 0.02; // PVI de 0 a 2
        double np = (t <= t_bt) ? t : (welge->getSwMedia() - swi); // Simplificação teórica [cite: 636]
        tPVI.append(t);
        Ed.append(np / (1.0 - swi)); // Eq. 3.19 [cite: 627]
    }
    ui->plotEfDesl->graph(0)->setData(tPVI, Ed);

    // Destacar o Breakthrough no gráfico
    ui->plotEfDesl->graph(1)->setData({t_bt}, {t_bt / (1.0 - swi)});

    // Replotar todos
    ui->plotFluxo->replot();
    ui->plotSvsX->replot();
    ui->plotVel->replot();
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

