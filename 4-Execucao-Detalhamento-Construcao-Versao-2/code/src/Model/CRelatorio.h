#ifndef CRELATORIO_H
#define CRELATORIO_H

#include <string>
#include <vector>

/**
 * @brief Classe responsável pela geração de relatórios e exportação para PDF.
 *
 * Utiliza o padrão "Builder" simplificado: acumula informações textuais (em formato HTML)
 * passo a passo e, ao final, renderiza um documento PDF utilizando o motor de impressão do Qt.
 *
 * @author Fabiane
 * @date Janeiro 2026
 */
class CRelatorio {
private:
    std::string _textoRelatorio; ///< Armazena o conteúdo do relatório em formato HTML.

public:
    /**
     * @brief Construtor padrão.
     * Inicializa o buffer de texto com o cabeçalho HTML básico.
     */
    CRelatorio();

    /**
     * @brief Destrutor.
     */
    ~CRelatorio();

    /**
     * @brief Adiciona o cabeçalho com os dados de entrada do reservatório e fluidos.
     *
     * @param comp Comprimento do sistema (m).
     * @param area Área de seção transversal (m²).
     * @param phi Porosidade (fração).
     * @param k Permeabilidade absoluta (mD ou m²).
     * @param angulo Ângulo de inclinação (graus).
     * @param mio Viscosidade do óleo.
     * @param miw Viscosidade da água.
     */
    void gerarCabecalho(double comp, double area, double phi, double k, double angulo, double mio, double miw);

    void registrarDiagnosticoFisico(double nrl, double m, double ng);

    /**
     * @brief Adiciona uma nota ou observação livre ao relatório (HTML).
     * @param notaHtml Texto em formato HTML a ser inserido no relatório.
     */
    void adicionarNota(const std::string& notaHtml);

    /**
     * @brief Registra qual modelo de permeabilidade foi utilizado.
     * @param nomeModelo Nome do modelo (ex: "Corey", "LET").
     * @param parametros String formatada com os parâmetros usados (ex: "No=2, Nw=2").
     */
    void registrarAnalisePermeabilidade(const std::string& nomeModelo, const std::string& parametros);

    /**
     * @brief Registra os resultados analíticos do método de Welge.
     * @param swFrente Saturação na frente de choque.
     * @param swMedia Saturação média atrás da frente.
     * @param tempoRuptura Tempo adimensional (PVI) ou real da ruptura.
     */
    void registrarResultadoWelge(double swFrente, double swMedia, double tempoRuptura);

    /**
     * @brief Adiciona uma tabela com eficiência de recuperação e fator de recuperação.
     * @param tempo Tempo atual da simulação.
     * @param fatorRecuperacao Fator de recuperação de óleo (FR).
     */
    void registrarEficiencia(double tempo, double fatorRecuperacao);

    /**
     * @brief Finaliza o documento e salva em arquivo PDF.
     *
     * Utiliza QTextDocument e QPdfWriter (Qt) para renderizar o HTML interno.
     *
     * @param caminho Caminho completo onde o arquivo .pdf será salvo.
     * @return true se salvo com sucesso, false caso contrário.
     */
    bool exportarParaPDF(const std::string& caminho);

    /**
     * @brief Limpa o conteúdo do relatório atual para iniciar um novo.
     */
    void limpar();
};

#endif
