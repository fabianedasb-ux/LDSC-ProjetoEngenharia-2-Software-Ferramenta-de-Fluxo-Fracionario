/**
 * @file MainWindow.cpp
 * @brief Implementação dos manipuladores de eventos e injeção gráfica.
 * @author Fabiane da Silva Barros
 * @date Janeiro 2026
 */

#include "Mainwindow.h"
#include "ui_Mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <algorithm>
#include <map>
//[bueno]
#include <QToolTip>
#include <QApplication>
#include <QClipboard>

// Constante global de precisão para contornos
static constexpr double SWI_EPS = 1e-3;

// Inclusão dos pacotes matemáticos de domínio
#include "CCurvasPermeabilidadeCorey.h"
#include "CCurvasPermeabilidadeLET.h"
#include "CCurvasPermeabilidadeChierici.h"
#include "CCurvasPermeabilidadeTabelada.h"
#include "ICurvasPermeabilidade.h"
#include "CRelatorio.h"

// --- Construtor e Configuração Inicial ---
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Forçar a janela a iniciar maximizada
    this->showMaximized();
    // ou trava a janela na resolução 1920 x 1080
    //this->setFixedSize(1920, 1080);

    // Ajuste ergonômico do botão de Tema
    ui->btnTema->setText("");
    ui->btnTema->setIconSize(QSize(24, 24));

    // Alocação da dependência estrutural do Controller
    simulador = new CSimulador();
    simulador->setModeloPermeabilidade(new CCurvasPermeabilidadeCorey());

    configurarGraficos();

    // Aplicação mandatório do tema inicial de carregamento
    aplicarTemaUI();
    atualizarCoresGraficos();

    // Alinhamento do stack de painéis com o menu suspenso
    ui->stkModelos->setCurrentIndex(ui->cbModeloPerm->currentIndex());
}

// --- Destrutor ---
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

    // Conecta o movimento do mouse de cada gráfico ao slot de leitura de coordenadas [bueno]
    connect(ui->plotFluxo, &QCustomPlot::mouseMove, this, &MainWindow::slotMostrarCoordenadasMouse);
    connect(ui->plotSvsX, &QCustomPlot::mouseMove, this, &MainWindow::slotMostrarCoordenadasMouse);
    connect(ui->plotEfDesl, &QCustomPlot::mouseMove, this, &MainWindow::slotMostrarCoordenadasMouse);

    // Conecta o clique do mouse para copiar coordenadas ao clicar com o botão direito
    connect(ui->plotFluxo, &QCustomPlot::mousePress, this, &MainWindow::slotCopiarCoordenadasMouse);
    connect(ui->plotSvsX, &QCustomPlot::mousePress, this, &MainWindow::slotCopiarCoordenadasMouse);
    connect(ui->plotEfDesl, &QCustomPlot::mousePress, this, &MainWindow::slotCopiarCoordenadasMouse);

}

void MainWindow::on_cbModeloPerm_currentIndexChanged(int index) {
    ui->stkModelos->setCurrentIndex(index);
}

void MainWindow::sincronizarDadosComSimulador() {
    // 1. Coleta e Conversão de Dados Escalares
    // reservatório
    double L     = ui->leComprimento->text().toDouble(); // 100 m
    //simulador->Reservatorio()->L(ui->leComprimento->text().toDouble());
    double A     = ui->leArea->text().toDouble(); // 1 m²
    double phi   = ui->lePorosidade->text().toDouble() / 100.0; // 20 %
    double ang   = ui->leAngulo->text().toDouble(); // 0 graus
    double vazao = ui->leVazao->text().toDouble(); // 1 m³/d
    double k     = ui->lePerm->text().toDouble(); // 100 mD
    // fluido
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

    // =========================================================
    // ETAPA 2: Configura o Modelo de Permeabilidade (Strategy)
    // =========================================================
    ICurvasPermeabilidade* modelo = nullptr;
    int indiceModelo = ui->cbModeloPerm->currentIndex();

    if (indiceModelo == 0) { // COREY
        QString arq = ui->lblArquivo->text();
        if (arq != "Arquivo" && !arq.isEmpty()) {
            auto corey = new CCurvasPermeabilidadeCorey();
            // Corrigir: os dados devem ser lidos para a interface!
            corey->carregarDados(arq.toStdString());

            // Bueno inicio
            // Puxando os dados do model para a interface
            ui->leCoreyNo->setText(QString::number(corey->getno()));
            ui->leCoreyNw->setText(QString::number(corey->getnw()));
            ui->leCoreySwi->setText(QString::number(corey->getSwi()));
            ui->leCoreySor->setText(QString::number(corey->getSor()));
            ui->leCoreyKrwm->setText(QString::number(corey->getKrw_max()));
            ui->leCoreyKrom->setText(QString::number(corey->getKro_max()));
            // fim

            modelo = corey;
        } else {
            double no = ui->leCoreyNo->text().toDouble();
            double nw = ui->leCoreyNw->text().toDouble();
            double swi = ui->leCoreySwi->text().toDouble();
            double sor = ui->leCoreySor->text().toDouble();
            double krwm = ui->leCoreyKrwm->text().toDouble();
            double krom = ui->leCoreyKrom->text().toDouble();
            modelo = new CCurvasPermeabilidadeCorey(krom, krwm, no, nw, swi, sor);
        }

    } else if (indiceModelo == 1) { // LET
        QString arq = ui->lblArquivo->text();
        if (arq != "Arquivo" && !arq.isEmpty()) {
            auto let = new CCurvasPermeabilidadeLET();
            let->carregarDados(arq.toStdString());

            // Bueno inicio
            // Puxando os dados do model para a interface
            ui->leLetLw->setText(QString::number(let->getSor()));
            ui->leLetEw->setText(QString::number(let->getEw()));
            ui->leLetTw->setText(QString::number(let->getTw()));
            ui->leLetLo->setText(QString::number(let->getLo()));
            ui->leLetEo->setText(QString::number(let->getEo()));
            ui->leLetTo->setText(QString::number(let->getTo()));
            ui->leLetSwir->setText(QString::number(let->getSwi()));//swir?
            ui->leLetSor->setText(QString::number(let->getSor()));
            // fim
            modelo = let;
        } else {
            double Lw = ui->leLetLw->text().toDouble();
            double Ew = ui->leLetEw->text().toDouble();
            double Tw = ui->leLetTw->text().toDouble();
            double Lo = ui->leLetLo->text().toDouble();
            double Eo = ui->leLetEo->text().toDouble();
            double To = ui->leLetTo->text().toDouble();
            double swi = ui->leLetSwir->text().toDouble();
            double sor = ui->leLetSor->text().toDouble();
            modelo = new CCurvasPermeabilidadeLET(Lw, Ew, Tw, Lo, Eo, To, swi, sor);
        }

    } else if (indiceModelo == 2) { // CHIERICI
        QString arq = ui->lblArquivo->text();
        if (arq != "Arquivo" && !arq.isEmpty()) {
            auto chierici = new CCurvasPermeabilidadeChierici();
            chierici->carregarDados(arq.toStdString());

            // Bueno inicio
            // Puxando os dados do model para a interface
            ui->leChiericiAw->setText(QString::number(chierici->getAw()));
            ui->leChiericiBw->setText(QString::number(chierici->getBw()));
            ui->leChiericiAo->setText(QString::number(chierici->getAo()));
            ui->leChiericiBo->setText(QString::number(chierici->getBo()));
            ui->leChiericiSwir->setText(QString::number(chierici->getSwi()));
            ui->leChiericiSor->setText(QString::number(chierici->getSor()));
            ui->leChiericiKrwMax->setText(QString::number(chierici->getkroMax()));
            ui->leChiericiKroMax->setText(QString::number(chierici->getkrwMax()));
            // fim
            modelo = chierici;
        } else {
            double Aw = ui->leChiericiAw->text().toDouble();
            double Bw = ui->leChiericiBw->text().toDouble();
            double Ao = ui->leChiericiAo->text().toDouble();
            double Bo = ui->leChiericiBo->text().toDouble();
            double swi = ui->leChiericiSwir->text().toDouble();
            double sor = ui->leChiericiSor->text().toDouble();
            double krwm = ui->leChiericiKrwMax->text().toDouble();
            double krom = ui->leChiericiKroMax->text().toDouble();
            modelo = new CCurvasPermeabilidadeChierici(Aw, Bw, Ao, Bo, swi, sor, krwm, krom);
        }

    } else { // TABELA
        QString arq = ui->lblArquivo->text();
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
    auto modelo = simulador->getModeloPermeabilidade();
    int indiceAtivo = ui->stkModelos->currentIndex();

    switch (indiceAtivo) {
    case 0: { // Corey
        auto corey = dynamic_cast<CCurvasPermeabilidadeCorey*>(modelo);
        if (corey && ui->lblArquivo->text() != "Arquivo" && !ui->lblArquivo->text().isEmpty())
            return corey->getSwi();
        return ui->leCoreySwi->text().toDouble();
    }
    case 1: { // LET
        auto let = dynamic_cast<CCurvasPermeabilidadeLET*>(modelo);
        if (let && ui->lblArquivo->text() != "Arquivo" && !ui->lblArquivo->text().isEmpty())
            return let->getSwi();
        return ui->leLetSwir->text().toDouble();
    }
    case 2: { // Chierici
        auto chierici = dynamic_cast<CCurvasPermeabilidadeChierici*>(modelo);
        if (chierici && ui->lblArquivo->text() != "Arquivo" && !ui->lblArquivo->text().isEmpty())
            return chierici->getSwi();
        return ui->leChiericiSwir->text().toDouble();
    }
    case 3: { // Tabela
        auto tabela = dynamic_cast<CCurvasPermeabilidadeTabelada*>(modelo);
        if (tabela) return tabela->getSwi();
        return 0.40; // numeros mágicos?
    }
    default:
        return 0.0; // numeros mágicos?
    }
}

double MainWindow::obterSaturacaoOleoResidualUI() const {
    auto modelo = simulador->getModeloPermeabilidade();
    int indiceAtivo = ui->stkModelos->currentIndex();

    switch (indiceAtivo) {
    case 0: { // Corey
        auto corey = dynamic_cast<CCurvasPermeabilidadeCorey*>(modelo);
        if (corey && ui->lblArquivo->text() != "Arquivo" && !ui->lblArquivo->text().isEmpty())
            return corey->getSor();
        return ui->leCoreySor->text().toDouble();
    }
    case 1: { // LET
        auto let = dynamic_cast<CCurvasPermeabilidadeLET*>(modelo);
        if (let && ui->lblArquivo->text() != "Arquivo" && !ui->lblArquivo->text().isEmpty())
            return let->getSor();
        return ui->leLetSor->text().toDouble();
    }
    case 2: { // Chierici
        auto chierici = dynamic_cast<CCurvasPermeabilidadeChierici*>(modelo);
        if (chierici && ui->lblArquivo->text() != "Arquivo" && !ui->lblArquivo->text().isEmpty())
            return chierici->getSor();
        return ui->leChiericiSor->text().toDouble();
    }
    case 3: { // Tabela
        auto tabela = dynamic_cast<CCurvasPermeabilidadeTabelada*>(modelo);
        if (tabela) return 1.0 - tabela->getSwMax();
        return 0.30;
    }
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

        double comp_fw = ui->leComprimento->text().toDouble();
        double phi_fw = ui->lePorosidade->text().toDouble() / 100.0;

        // 2. Imprime no Log o Número de Rapoport-Leas calculado
        ui->txtLog->appendHtml(QString("N. Rapoport-Leas: %1").arg(calc->calcularRapoportLeas(comp_fw, phi_fw, 0.03), 0, 'f', 2));

        // 3. Define a saturação para exibição e calcula as constantes globais
        double sw_report = 1 - sor;// bug? não usa?
        double M_sw = calc->calcularM0();// bug? não usa?
        double Ng_sw = calc->calcularNg0();// bug? não usa?

        auto modelo = simulador->getModeloPermeabilidade();// bug? não usa?


        QVector<double> xFw(101), yFw(101);

        for (int i = 0; i <= 100; ++i) {
            double sw = i / 100.0;
            xFw[i] = sw;
            yFw[i] = calc->calcularFw(sw); // Usa a Eq. 3.10 corrigida
        }

        ui->plotFluxo->graph(0)->setData(xFw, yFw);
        ui->plotFluxo->xAxis->setRange(0, 1.0);
        ui->plotFluxo->graph(0)->rescaleAxes();
        // pequena margem visual
        double yMin = ui->plotFluxo->yAxis->range().lower;
        double yMax = ui->plotFluxo->yAxis->range().upper;
        double margin = 0.05 * (yMax - yMin);
        ui->plotFluxo->yAxis->setRange(yMin - margin, yMax + margin);
        ui->plotFluxo->replot();

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Erro", e.what());
    }
}


// --- SIMULAÇÃO PRINCIPAL ---
void MainWindow::on_btnPlotarSolucao_clicked() {
    try {
        sincronizarDadosComSimulador();

        double pvi_input = ui->lePVI->text().toDouble();
        if (pvi_input <= 0.0) {
            throw std::runtime_error("Informe um tempo válido no campo Tempo.");
        }

        double L = ui->leComprimento->text().toDouble();
        double A = ui->leArea->text().toDouble();
        double phi = ui->lePorosidade->text().toDouble() / 100.0;

        if (A <= 0.0 || phi <= 0.0 || L <= 0.0) {
            throw std::runtime_error("Verifique Comprimento, Área e Porosidade (devem ser > 0).");
        }

        // CORREÇÃO: Coleta as condições de contorno e passa para o pipeline do simulador
        double swi = obterSaturacaoInicialUI();
        double sor = obterSaturacaoOleoResidualUI();
        double sw_max = 1.0 - sor;

        simulador->executarSimulacao(pvi_input, swi, sw_max);

        plotarResultados();
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Erro", e.what());
    }
}

void MainWindow::plotarResultados() {
    auto calc = simulador->getCalculadora();
    auto welge = simulador->getWelge();

    double pvi_input = ui->lePVI->text().toDouble();// bug? não usa?
    double swi = obterSaturacaoInicialUI();
    double sor = obterSaturacaoOleoResidualUI();
    double sw_max = 1.0 - sor;

    // Executa Welge passando os parâmetros reais
    welge->calcularTangente(calc, swi, sw_max);

    double swFrente = welge->getSwFrente();
    double vel_choque = welge->getInclinacaoChoque();
    double t_bt = (vel_choque > 1e-6) ? (1.0 / vel_choque) : 1.0;

    // -----------------------------------------------------------------
    // 1. PERFIL DE SATURAÇÃO (Sw vs xD)
    // -----------------------------------------------------------------
    QVector<double> xD, Sw;

    // Busca os dados oficiais já calculados, filtrados (Entropia) e ordenados pelo CSolver/CMalha
    const std::vector<CCelula>& malha_celulas = simulador->getMalha()->getCelulas();

    for (const CCelula& celula : malha_celulas) {
        xD.append(celula.getPosicao());
        Sw.append(celula.getSaturacao());
    }

    ui->plotSvsX->graph(0)->setData(xD, Sw);
    ui->plotSvsX->xAxis->setRange(0.0, 1.0);
    ui->plotSvsX->yAxis->setRange(0.0, 1.05);

    // -----------------------------------------------------------------
    // 2. TANGENTE DE WELGE NO FLUXO FRACIONÁRIO
    // -----------------------------------------------------------------
    if (swFrente > swi + 1e-6) {
        ui->plotFluxo->graph(1)->setData({swi, welge->getSwMedia()}, {calc->calcularFw(swi), 1.0});
        ui->plotFluxo->rescaleAxes();
        double yMinF = ui->plotFluxo->yAxis->range().lower;
        double yMaxF = ui->plotFluxo->yAxis->range().upper;
        ui->plotFluxo->yAxis->setRange(yMinF - 0.05*(yMaxF-yMinF), yMaxF + 0.05*(yMaxF-yMinF));
    } else {
        if (ui->plotFluxo->graphCount() > 1 && ui->plotFluxo->graph(1)->data()) {
            ui->plotFluxo->graph(1)->data()->clear();
        }
    }

    // -----------------------------------------------------------------
    // 3. EFICIÊNCIA DE DESLOCAMENTO (Ed vs PVI)
    // -----------------------------------------------------------------
    std::map<double, double> curvaEd;

    for (double t = 0.0; t <= t_bt; t += 0.01) {
        curvaEd[t] = t / (1.0 - swi);
    }

    for (double s = swFrente + 0.001; s <= sw_max; s += 0.001) {
        double deriv = calc->calcularDerivadaFw(s);

        if (deriv > 1e-6) {
            double t_saida = 1.0 / deriv;

            if (t_saida >= t_bt && t_saida <= 3.0) {
                double fw_s = calc->calcularFw(s);
                double sw_media = s + (1.0 - fw_s) * t_saida;
                double ed_atual = (sw_media - swi) / (1.0 - swi);
                curvaEd[t_saida] = ed_atual;
            }
        }
    }

    QVector<double> tPVI, Ed;
    double max_ed = 0.0;

    for (auto const& par : curvaEd) {
        double t = par.first;
        double ed = par.second;

        if (ed >= max_ed) {
            max_ed = ed;
            tPVI.append(t);
            Ed.append(ed);
        }
    }

    if (!tPVI.isEmpty() && tPVI.last() < 3.0) {
        tPVI.append(3.0);
        Ed.append(Ed.last());
    }

    ui->plotEfDesl->graph(0)->setData(tPVI, Ed);

    double ed_bt = t_bt / (1.0 - swi);
    ui->plotEfDesl->graph(1)->setData({t_bt}, {ed_bt});

    ui->plotEfDesl->graph(0)->rescaleAxes();
    ui->plotEfDesl->xAxis->setRange(0.0, 3.0);
    double yMinE = ui->plotEfDesl->yAxis->range().lower;
    double yMaxE = ui->plotEfDesl->yAxis->range().upper;
    ui->plotEfDesl->yAxis->setRange(std::max(0.0, yMinE - 0.05*(yMaxE-yMinE)), yMaxE + 0.05*(yMaxE-yMinE));

    ui->plotFluxo->replot();
    ui->plotSvsX->replot();
    ui->plotEfDesl->replot();
}

// --- Carregamento de Arquivos ---

// void MainWindow::on_btlCarregarCorey_clicked() {
//     QString path = QFileDialog::getOpenFileName(this, "Abrir Corey", "", "Text Files (*.txt)");
//     if(path.isEmpty()) return;
//     ui->lblArquivo->setText(path);

//     // Opcional: Carregar do arquivo para os LineEdits agora
//     try {
//         CCurvasPermeabilidadeCorey temp;
//         temp.carregarDados(path.toStdString());
//         // Aqui poderíamos ter getters na classe Corey para preencher a tela...
//         // Como não implementamos getters nas classes de curva, apenas armazenamos o path.
//         QMessageBox::information(this, "Sucesso", "Arquivo selecionado. Clique em Solução para usar.");
//     } catch (const std::exception& e) {
//         QMessageBox::critical(this, "Erro", e.what());
//     }
// }

// void MainWindow::on_btnCarregarLET_clicked() {
//     QString path = QFileDialog::getOpenFileName(this, "Abrir LET", "", "Text Files (*.txt)");
//     if(!path.isEmpty()) ui->lblArquivo->setText(path);
// }

// void MainWindow::on_btnCarregarChierici_clicked() {
//     QString path = QFileDialog::getOpenFileName(this, "Abrir Chierici", "", "Text Files (*.txt)");
//     if(!path.isEmpty()) ui->lblArquivo->setText(path);
// }

// void MainWindow::on_btnCarregarTabela_clicked() {
//     QString path = QFileDialog::getOpenFileName(this, "Abrir Tabela", "", "Text Files (*.txt)");
//     if(!path.isEmpty()) ui->lblArquivo->setText(path);
// }

void MainWindow::on_btlCarregar_clicked() {

    int indiceModelo = ui->cbModeloPerm->currentIndex();
    QString path;
    if (indiceModelo == 0) { // COREY
            path = QFileDialog::getOpenFileName(this, "Abrir Corey", "", "Text Files (*.txt)");
            if(path.isEmpty()) return;
            ui->lblArquivo->setText(path);

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
    } else if (indiceModelo == 1) { // LET
            path = QFileDialog::getOpenFileName(this, "Abrir LET", "", "Text Files (*.txt)");
            if(!path.isEmpty()) ui->lblArquivo->setText(path);
    } else if (indiceModelo == 2) { // CHIERICI
            path = QFileDialog::getOpenFileName(this, "Abrir Chierici", "", "Text Files (*.txt)");
            if(!path.isEmpty()) ui->lblArquivo->setText(path);
    } else { // TABELA
            path = QFileDialog::getOpenFileName(this, "Abrir Tabela", "", "Text Files (*.txt)");
            if(!path.isEmpty()) ui->lblArquivo->setText(path);
    }
}


void MainWindow::on_btnRelatorio_clicked() {
    try {
        QString caminho = QFileDialog::getSaveFileName(this, "Salvar Relatório Executivo", "", "PDF (*.pdf)");
        if (caminho.isEmpty()) return;

        CRelatorio rel;

        // 1. Coleta os parâmetros estáticos
        double L = ui->leComprimento->text().toDouble();
        double A = ui->leArea->text().toDouble();
        double phi = ui->lePorosidade->text().toDouble() / 100.0;
        double k = ui->lePerm->text().toDouble();
        double angulo = ui->leAngulo->text().toDouble();
        double mi_o = ui->leViscOleo->text().toDouble();
        double mi_w = ui->leViscAgua->text().toDouble();

        rel.gerarCabecalho(L, A, phi, k, angulo, mi_o, mi_w);

        // 2. Coleta dados da calculadora física
        auto calc = simulador->getCalculadora();
        double nrl = calc->calcularRapoportLeas(L, phi, 0.03); // Tensão interfacial padrão
        double M = calc->calcularM0();
        double Ng = calc->calcularNg0();

        rel.registrarDiagnosticoFisico(nrl, M, Ng);

        // 3. Métricas Avançadas de Welge e Ruptura (BT)
        auto welge = simulador->getWelge();
        double swi = obterSaturacaoInicialUI();
        double swFrente = welge->getSwFrente();
        double swMedia = welge->getSwMedia();
        double vel_choque = welge->getInclinacaoChoque();
        double t_bt = (vel_choque > 1e-6) ? (1.0 / vel_choque) : 1.0;

        // Cálculo analítico da Eficiência de Deslocamento no exato instante de Breakthrough
        double ed_bt = (swMedia - swi) / (1.0 - swi);

        rel.registrarResultadoWelge(swFrente, swMedia, t_bt, ed_bt);

        // 4. Parâmetros dinâmicos do tempo digitado na interface
        double pvi_input = ui->lePVI->text().toDouble();
        // Saturação média atual depende se já passou do BT ou não
        double sw_media_atual = swMedia;
        if (pvi_input > t_bt) {
            double target_deriv = 1.0 / pvi_input;
            double sor = obterSaturacaoOleoResidualUI();
            double sw_max = 1.0 - sor;
            double sw_out = swFrente;
            for (double s = swFrente; s <= sw_max; s += 0.001) {
                if (calc->calcularDerivadaFw(s) <= target_deriv) {
                    sw_out = s;
                    break;
                }
            }
            double fw_out = calc->calcularFw(sw_out);
            sw_media_atual = sw_out + (1.0 - fw_out) * pvi_input;
        } else {
            sw_media_atual = swi + pvi_input;
        }
        double ed_atual = (sw_media_atual - swi) / (1.0 - swi);
        if(ed_atual > 1.0 - obterSaturacaoOleoResidualUI() - swi) ed_atual = 1.0 - obterSaturacaoOleoResidualUI() - swi;

        rel.registrarEficiencia(pvi_input, ed_atual);

        // =========================================================
        // 5. CAPTURA DOS GRÁFICOS DA INTERFACE (MÁGICA VISUAL)
        // Extrai os Pixmaps com tamanho adequado para o documento A4
        // =========================================================
        QPixmap pixFluxo = ui->plotFluxo->toPixmap(500, 350);
        QPixmap pixSaturacao = ui->plotSvsX->toPixmap(500, 350);
        QPixmap pixEficiencia = ui->plotEfDesl->toPixmap(500, 350);

        rel.registrarGraficos(pixFluxo, pixSaturacao, pixEficiencia);

        // 6. Exportação definitiva
        rel.exportarParaPDF(caminho.toStdString());
        QMessageBox::information(this, "Sucesso", "Relatório PDF gerado com sucesso.");

    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Erro ao Gerar Relatório", e.what());
    }
}

// --- Lógica de Temas Atualizada e Polida ---

// Esta função define a "estrutura" visual que não muda (Fonte e Cantos Arredondados)
QString MainWindow::gerarEstiloBaseUI() {
    return R"(
        /* Define a fonte e cantos para QUASE tudo na janela */
        QWidget {
            font-family: 'Segoe UI', 'Helvetica Neue', sans-serif;
            font-size: 10pt;
            border-radius: 6px; /* Cantos arredondados padrão universal */
        }

        /* Títulos e Labels mais fortes */
        QLabel {
            font-weight: bold;
            background: transparent; /* Garante que labels não tenham fundo feio */
        }

        /* Inputs e Combos arredondados e com respiro */
        QLineEdit, QComboBox, QSpinBox {
            border: 1px solid; /* A cor muda no tema */
            padding: 5px;
            min-height: 20px;
        }

        /* Botões com cantos bem arredondados e padding */
        QPushButton {
            border: none;
            padding: 8px 15px;
            font-weight: bold;
            min-width: 80px;
        }

        /* TextEdit do Log arredondado */
        QTextEdit {
            border: 1px solid;
            border-radius: 6px;
        }
    )";
}

void MainWindow::on_btnTema_clicked() {
    isDarkMode = !isDarkMode; // Apenas inverte o estado e manda aplicar
    aplicarTemaUI();
    atualizarCoresGraficos();
}

void MainWindow::aplicarTemaUI() {
    QString estiloFinal = gerarEstiloBaseUI();

    if (isDarkMode) {
        // --- CORES TEMA ESCURO ---
        // Caminho atualizado com a pasta "icones"
        ui->btnTema->setIcon(QIcon(":/icones/lua.png"));

        estiloFinal += R"(
            QMainWindow, QWidget {
                background-color: #282c34;
                color: #abb2bf;
            }
            QLabel { color: #abb2bf; }

            QLineEdit, QComboBox, QTextEdit, QSpinBox, QDoubleSpinBox {
                background-color: #21252b;
                color: #abb2bf;
                border-color: #181a1f;
            }
            QLineEdit:focus, QComboBox:focus { border-color: #61afef; }

            QPushButton {
                background-color: #3e4451;
                color: #ffffff;
            }
            QPushButton:hover { background-color: #61afef; color: #282c34; }
            QPushButton:pressed { background-color: #528bff; }

            QTextEdit { color: #98c379; }
        )";
    } else {
        // --- CORES TEMA CLARO ---
        // Caminho atualizado com a pasta "icones"
        ui->btnTema->setIcon(QIcon(":/icones/sol.png"));

        estiloFinal += R"(
            QMainWindow, QWidget {
                background-color: #f5f5f5;
                color: #333333;
            }
            QLabel { color: #333333; }

            QLineEdit, QComboBox, QTextEdit, QSpinBox, QDoubleSpinBox {
                background-color: #ffffff;
                color: #333333;
                border-color: #cccccc;
            }
            QLineEdit:focus, QComboBox:focus { border-color: #0078d7; }

            QPushButton {
                background-color: #e1e1e1;
                color: #333333;
            }
            QPushButton:hover { background-color: #0078d7; color: #ffffff; }
            QPushButton:pressed { background-color: #005a9e; color: #ffffff; }

            QTextEdit { color: #006400; }
        )";
    }

    this->setStyleSheet(estiloFinal);
}

// *** IMPORTANTE: Mantenha a função atualizarCoresGraficos() exatamente
// como corrigimos na resposta anterior (complots << ui->plot...) ***

void MainWindow::atualizarCoresGraficos() {
    // Cores baseadas no tema
    QColor corFundo = isDarkMode ? QColor("#282c34") : QColor("#ffffff");
    QColor corEixos = isDarkMode ? QColor("#abb2bf") : QColor("#000000");
    QColor corGrid  = isDarkMode ? QColor("#3e4451") : QColor("#e0e0e0");

    // Lista de todos os gráficos na sua tela
    QList<QCustomPlot*> plots = {ui->plotFluxo, ui->plotSvsX, ui->plotEfDesl};

    for (QCustomPlot* plot : plots) {
        plot->setBackground(QBrush(corFundo));

        // Eixo X
        plot->xAxis->setBasePen(QPen(corEixos));
        plot->xAxis->setTickPen(QPen(corEixos));
        plot->xAxis->setSubTickPen(QPen(corEixos));
        plot->xAxis->setTickLabelColor(corEixos);
        plot->xAxis->setLabelColor(corEixos);
        plot->xAxis->grid()->setPen(QPen(corGrid, 1, Qt::DotLine));
        plot->xAxis->grid()->setZeroLinePen(Qt::NoPen);

        // Eixo Y
        plot->yAxis->setBasePen(QPen(corEixos));
        plot->yAxis->setTickPen(QPen(corEixos));
        plot->yAxis->setSubTickPen(QPen(corEixos));
        plot->yAxis->setTickLabelColor(corEixos);
        plot->yAxis->setLabelColor(corEixos);
        plot->yAxis->grid()->setPen(QPen(corGrid, 1, Qt::DotLine));
        plot->yAxis->grid()->setZeroLinePen(Qt::NoPen);

        // Bordas superiores e direitas
        plot->xAxis2->setBasePen(QPen(corEixos));
        plot->yAxis2->setBasePen(QPen(corEixos));

        plot->replot();
    }
}

void MainWindow::slotMostrarCoordenadasMouse(QMouseEvent *event) {
    // Identifica qual dos 3 gráficos enviou o evento
    QCustomPlot *plot = qobject_cast<QCustomPlot*>(sender());
    if (!plot) return;

    // Converte os pixels do cursor para as coordenadas reais do gráfico (X e Y)
    double x = plot->xAxis->pixelToCoord(event->pos().x());
    double y = plot->yAxis->pixelToCoord(event->pos().y());

    // Formata o texto com 4 casas decimais
    QString texto = QString("X: %1 | Y: %2").arg(x, 0, 'f', 4).arg(y, 0, 'f', 4);

    // Opção 1: Exibe na QStatusBar (barra inferior da tela)
    ui->statusbar->showMessage(texto);

    // Opção 2: Exibe o balãozinho (Tooltip) diretamente ao lado do ponteiro do mouse
    QToolTip::showText(QCursor::pos(), texto, plot);
}

void MainWindow::slotCopiarCoordenadasMouse(QMouseEvent *event) {
    //
    if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton) {
        QCustomPlot *plot = qobject_cast<QCustomPlot*>(sender());
        if (!plot) return;

        // Converte a posição em pixels do clique para as coordenadas reais do gráfico
        double x = plot->xAxis->pixelToCoord(event->pos().x());
        double y = plot->yAxis->pixelToCoord(event->pos().y());

        // Formata o texto. O caractere '\t' (TAB) facilita colar direto no Excel
        QString textoCopiado = QString("%1\t%2").arg(x, 0, 'g', 6).arg(y, 0, 'g', 6);

        // Envia o texto para a Área de Transferência do sistema operacional e anota na barra de status
        if (event->button() == Qt::RightButton) {
            QApplication::clipboard()->setText(textoCopiado);
            // Mensagem de feedback temporária na StatusBar (desaparece após 3 segundos)
            ui->statusbar->showMessage(QString("Copiado para a área de transferência: X=%1, Y=%2")
                                           .arg(x, 0, 'f', 4).arg(y, 0, 'f', 4), 3000);
        }
        // Envia para arquivo de log
        else if (event->button() == Qt::RightButton) {
            ui->txtLog->appendHtml(textoCopiado);
            ui->txtLog->show();
      }
    }
}
