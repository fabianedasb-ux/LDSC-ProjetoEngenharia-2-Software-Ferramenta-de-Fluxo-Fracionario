#ifndef CRELATORIO_H
#define CRELATORIO_H

/**
 * @file CRelatorio.h
 * @brief Definição da classe CRelatorio para exportação de dados analíticos.
 */

#include <string>
#include <QString>
#include <QPixmap>

/**
 * @class CRelatorio
 * @brief Gerenciador de relatórios técnicos e exportação PDF.
 *
 * Esta classe é responsável por agregar os dados estáticos, os diagnósticos adimensionais,
 * os resultados numéricos do choque de Welge e os gráficos gerados pela interface.
 * O motor interno utiliza a sintaxe HTML5 e CSS3 inline para construir o layout do documento.
 * Posteriormente, o relatório é renderizado vetorizado em formato PDF através
 * das classes nativas QTextDocument e QPdfWriter do Qt.
 *
 * @author Fabiane da Silva Barros
 * @date Janeiro 2026
 */
class CRelatorio {
private:
    std::string _textoRelatorio; ///< String cumulativa contendo o código fonte HTML/CSS do documento.

public:
    /**
     * @brief Construtor padrão da classe CRelatorio.
     * Inicializa a variável de texto e injeta a folha de estilo CSS (limpar).
     */
    CRelatorio();

    /**
     * @brief Destrutor virtual.
     */
    ~CRelatorio();

    /**
     * @brief Reinicializa o documento HTML.
     * Apaga dados antigos e injeta o cabeçalho base contendo os estilos CSS,
     * fontes (Segoe UI) e formatações rigorosas de tabelas e legendas (Padrão ABNT).
     */
    void limpar();

    /**
     * @brief Registra a tabela de propriedades estáticas do reservatório.
     * @param comp Comprimento (L).
     * @param area Área transversal (A).
     * @param phi Porosidade efetiva.
     * @param k Permeabilidade absoluta.
     * @param angulo Mergulho estrutural.
     * @param mio Viscosidade do óleo.
     * @param miw Viscosidade da água.
     */
    void gerarCabecalho(double comp, double area, double phi, double k, double angulo, double mio, double miw);

    /**
     * @brief Registra os adimensionais de diagnóstico e emite o aviso do critério capilar.
     * @param nrl Número de Rapoport-Leas.
     * @param m Razão de mobilidade no ponto final (M^0).
     * @param ng Número de gravidade (Ng).
     */
    void registrarDiagnosticoFisico(double nrl, double m, double ng);

    /**
     * @brief Destaca os resultados críticos da técnica de Welge no instante de ruptura (Breakthrough).
     * @param swFrente Saturação da água na frente de choque.
     * @param swMedia Saturação média da zona varrida.
     * @param tempoRuptura Tempo de injeção (PVI) exato no momento do breakthrough.
     * @param edBt Eficiência de deslocamento volumétrico na ruptura.
     */
    void registrarResultadoWelge(double swFrente, double swMedia, double tempoRuptura, double edBt);

    /**
     * @brief Registra o status temporal do instante em que o relatório foi gerado.
     * @param tempo Tempo atual de injeção analisado (PVI).
     * @param fatorRecuperacao Recuperação atual baseada no volume poroso.
     */
    void registrarEficiencia(double tempo, double fatorRecuperacao);

    /**
     * @brief Converte e embuti os gráficos do QCustomPlot diretamente no documento HTML.
     *
     * Os objetos `QPixmap` são codificados em memória para fluxos binários PNG e,
     * em seguida, transformados em strings Base64. Isso permite injetar a imagem
     * como uma URI de dados (`<img src='data:image/png;base64,...'>`) sem depender
     * de arquivos temporários no disco rígido.
     *
     * @param pixFluxo Captura de tela da curva do fluxo fracionário.
     * @param pixSaturacao Captura de tela do perfil de saturação de água.
     * @param pixEficiencia Captura de tela do histórico de eficiência.
     */
    void registrarGraficos(const QPixmap& pixFluxo, const QPixmap& pixSaturacao, const QPixmap& pixEficiencia);

    /**
     * @brief Anexa anotações customizadas ao fim do relatório.
     * @param notaHtml String contendo texto ou marcação HTML extra inserida pelo usuário.
     */
    void adicionarNota(const std::string& notaHtml);

    /**
     * @brief Renderiza e exporta o arquivo PDF em disco.
     *
     * Finaliza a sintaxe HTML e utiliza o motor de renderização `QTextDocument` em conjunto
     * com o `QPdfWriter` para formatar e paginar o relatório no padrão ISO A4.
     *
     * @param caminho Caminho absoluto alvo para gravação do arquivo `.pdf`.
     * @return true se o arquivo for gravado com sucesso, false em caso de falha de I/O.
     */
    bool exportarParaPDF(const std::string& caminho);
};

#endif // CRELATORIO_H