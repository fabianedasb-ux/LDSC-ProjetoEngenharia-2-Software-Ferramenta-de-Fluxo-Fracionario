#ifndef CCELULA_H
#define CCELULA_H

/**
 * @brief Representa um ponto discreto (volume elementar) no domínio do reservatório.
 *
 * Na abordagem analítica, a célula funciona como um container de dados que armazena
 * o estado de uma saturação específica (Sw) e sua posição (x) correspondente em um
 * determinado tempo (t).
 *
 * @author Fabiane Barros
 * @date Janeiro 2026
 */
class CCelula {
private:
    double _saturacao;      ///< Saturação de água (Sw) neste ponto.
    double _posicao;        ///< Posição espacial (x) no reservatório [m].
    double _derivadaFluxo;  ///< Velocidade adimensional da onda (dfw/dSw).

public:
    /**
     * @brief Construtor padrão.
     * Inicializa com valores zerados.
     */
    CCelula();

    /**
     * @brief Construtor parametrizado.
     *
     * @param sw Valor da saturação de água.
     * @param x Posição inicial no espaço.
     * @param dfw Valor da derivada do fluxo fracionário (velocidade da onda).
     */
    CCelula(double sw, double x, double dfw);

    /**
     * @brief Destrutor virtual.
     */
    virtual ~CCelula();

    // --- Getters (Métodos de Acesso) ---

    /**
     * @brief Retorna a saturação de água da célula.
     * @return Sw (0 a 1).
     */
    double getSaturacao() const;

    /**
     * @brief Retorna a posição atual da célula.
     * @return Posição x.
     */
    double getPosicao() const;

    /**
     * @brief Retorna a derivada do fluxo fracionário armazenada.
     * @return dfw/dSw.
     */
    double getDerivadaFluxo() const;

    // --- Setters (Métodos de Modificação) ---

    void setSaturacao(double sw);
    void setPosicao(double x);
    void setDerivadaFluxo(double dfw);
};

#endif
