#ifndef CCELULA_H
#define CCELULA_H

/**
 * @file CCelula.h
 * @brief Definição da classe CCelula.
 */

/**
 * @class CCelula
 * @brief Representa um ponto discreto (volume elementar) no domínio do reservatório.
 *
 * Na abordagem analítica, a célula funciona como um container de dados que armazena
 * o estado de uma saturação específica (Sw) e sua posição (x) correspondente em um
 * determinado tempo (t).
 *
 * @author Fabiane da Silva Barros
 * @date Janeiro 2026
 */
class CCelula {
private:
    double _saturacao;      ///< Saturação de água (Sw) neste ponto.
    double _posicao;        ///< Posição espacial (x) no reservatório [m].
    double _derivadaFluxo;  ///< Velocidade adimensional da onda (dfw/dSw).

public:
    /**
     * @brief Construtor padrão da classe CCelula.
     * Inicializa todos os atributos numéricos com valor zero.
     */
    CCelula();

    /**
     * @brief Construtor parametrizado da classe CCelula.
     *
     * @param sw Valor da saturação de água da célula.
     * @param x Posição inicial no espaço (adimensional ou em metros).
     * @param dfw Valor da derivada do fluxo fracionário (velocidade da onda associada).
     */
    CCelula(double sw, double x, double dfw);

    /**
     * @brief Destrutor virtual da classe CCelula.
     */
    virtual ~CCelula();

    // --- Getters (Métodos de Acesso) ---

    /**
     * @brief Retorna a saturação de água atual da célula.
     * @return O valor da saturação (Sw) compreendido entre 0 e 1.
     */
    double getSaturacao() const;

    /**
     * @brief Retorna a posição espacial atual da célula.
     * @return O valor da posição (x).
     */
    double getPosicao() const;

    /**
     * @brief Retorna a derivada do fluxo fracionário armazenada na célula.
     * @return O valor da taxa de variação (dfw/dSw).
     */
    double getDerivadaFluxo() const;

    // --- Setters (Métodos de Modificação) ---

    /**
     * @brief Define uma nova saturação de água para a célula.
     * @param sw O novo valor da saturação de água.
     */
    void setSaturacao(double sw);

    /**
     * @brief Define uma nova posição espacial para a célula.
     * @param x O novo valor da posição.
     */
    void setPosicao(double x);

    /**
     * @brief Define uma nova derivada de fluxo para a célula.
     * @param dfw O novo valor da derivada do fluxo fracionário.
     */
    void setDerivadaFluxo(double dfw);
};

#endif // CCELULA_H