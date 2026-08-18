#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/**
 * @file MainWindow.h
 * @brief Definição da classe MainWindow, a interface gráfica principal do simulador.
 */

#include <QMainWindow>
// Bueno
#include <QMouseEvent> // Captura do mouse e uso de seus eventos(como mover o mouse sobre o gráfico)
#include <map>
#include "CSimulador.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @class MainWindow
 * @brief Camada de Apresentação (View) do simulador analítico.
 *
 * Esta classe herda de QMainWindow e é responsável por toda a interação direta
 * com o usuário. Ela coleta os parâmetros petrofísicos e de fluidos, repassa-os
 * ao Controlador (CSimulador), e gerencia a atualização visual dos painéis de
 * gráficos (QCustomPlot) e dos relatórios.
 *
 * Além disso, a classe implementa um gerenciador dinâmico de folhas de estilo (QSS)
 * para alternância em tempo de execução entre os temas Claro e Escuro, garantindo
 * conforto visual ergonômico.
 *
 * @author Fabiane da Silva Barros
 * @date Janeiro 2026
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Construtor padrão da interface gráfica.
     * @param parent Widget pai (opcional).
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destrutor virtual.
     * Libera a interface de usuário (UI) e a instância do simulador alocada.
     */
    ~MainWindow();

private slots:
    // --- Gatilhos de Ação (Slots) ---

    /**
     * @brief Slot acionado para calcular e plotar estritamente a curva de fluxo fracionário.
     */
    void on_btnPlotarFw_clicked();

    /**
     * @brief Slot acionado para executar a varredura completa da simulação (MOC + Welge).
     */
    void on_btnPlotarSolucao_clicked();

    /**
     * @brief Mostrar coordenadas do mouse no grafico [bueno]
     */
    void slotMostrarCoordenadasMouse(QMouseEvent *event);

    /**
     * @brief Copiar coordenadas do mouse no grafico [bueno]
     */
    void slotCopiarCoordenadasMouse(QMouseEvent *event);

    /**
     * @brief Exportar grafico [bueno]
     */
    void slotExportarGrafico(QMouseEvent *event);

    /**
     * @brief Slot acionado para invocar a exportação do documento executivo em PDF.
     */
    void on_btnRelatorio_clicked();

    /**
     * @brief Slot acionado na alteração da aba (StackedWidget) do modelo de permeabilidade.
     * @param index Índice da aba selecionada (0: Corey, 1: LET, 2: Chierici, 3: Tabela).
     */
    void on_cbModeloPerm_currentIndexChanged(int index);

    // --- Slots de Manipulação de Arquivos ---
    void on_btlCarregar_clicked();
    // void on_btlCarregarCorey_clicked();
    // void on_btnCarregarLET_clicked();
    // void on_btnCarregarChierici_clicked();
    // void on_btnCarregarTabela_clicked();

    /**
     * @brief Slot acionado para alternar entre os Modos Claro e Escuro da UI.
     */
    void on_btnTema_clicked();

    void on_btnAjuda_clicked();

    void on_btnSobre_clicked();

private:
    Ui::MainWindow *ui;         ///< Ponteiro gerenciado pelo Qt contendo os elementos do XML (.ui).
    CSimulador *simulador;      ///< Ponteiro para o Controlador MVC principal.

    // --- Métodos Auxiliares Internos ---

    /**
     * @brief Coleta todos os inputs textuais da tela e sincroniza com o Controlador.
     * @throws std::runtime_error Se algum formato de entrada for inválido.
     */
    void sincronizarDadosComSimulador();

    /**
     * @brief Configura e inicializa os eixos e propriedades visuais do QCustomPlot.
     */
    void configurarGraficos();

    /**
     * @brief Atualiza a plotagem dos resultados após o processamento do CSolver.
     */
    void plotarResultados();

    /**
     * @brief Extrai a saturação residual de óleo baseada na aba ativa.
     * @return Valor de Sor.
     */
    double obterSaturacaoOleoResidualUI() const;

    /**
     * @brief Extrai a saturação irredutível de água baseada na aba ativa.
     * @return Valor de Swir.
     */
    double obterSaturacaoInicialUI() const;

    // --- Lógica de Temas (QSS) ---
    bool isDarkMode = false;    ///< Sinalizador de estado do tema visual (false = Modo Claro).

    /**
     * @brief Aplica as folhas de estilo (QSS) pertinentes aos widgets da tela.
     */
    void aplicarTemaUI();

    /**
     * @brief Refaz a renderização de cores do QCustomPlot (Fundo, eixos, malha) conforme o tema.
     */
    void atualizarCoresGraficos();

    /**
     * @brief Retorna as regras estruturais e de layout invariáveis da aplicação em QSS.
     * @return String contendo a folha de estilo base.
     */
    QString gerarEstiloBaseUI();
};

#endif // MAINWINDOW_H