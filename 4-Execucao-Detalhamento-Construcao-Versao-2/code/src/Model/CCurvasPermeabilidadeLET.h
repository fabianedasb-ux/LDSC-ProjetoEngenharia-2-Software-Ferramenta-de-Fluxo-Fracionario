#ifndef CCURVASPERMEABILIDADELET_H
#define CCURVASPERMEABILIDADELET_H

/**
 * @file CCurvasPermeabilidadeLET.h
 * @brief Definição da classe CCurvasPermeabilidadeLET.
 */

#include "ICurvasPermeabilidade.h"
#include <cmath>
#include <string>

/**
 * @class CCurvasPermeabilidadeLET
 * @brief Implementação empírica do modelo de permeabilidade relativa LET.
 *
 * O modelo LET (Lombez, Escovedo e Trindade, 2008) é uma correlação matemática
 * altamente flexível. Ele utiliza três parâmetros empíricos (L, E, T) para descrever
 * com precisão a forma (shape), elevação (elevation) e translação (translation)
 * das curvas de permeabilidade relativa, ajustando-se a dados laboratoriais complexos.
 *
 * As equações fundamentais são dadas por:
 * - \f$ k_{rw} = \frac{(S_{wn})^{L_w}}{(S_{wn})^{L_w} + E_w \cdot (1 - S_{wn})^{T_w}} \f$
 * - \f$ k_{ro} = \frac{(1 - S_{wn})^{L_o}}{(1 - S_{wn})^{L_o} + E_o \cdot (S_{wn})^{T_o}} \f$
 *
 * @author Fabiane da Silva Barros
 * @date Janeiro 2026
 */
class CCurvasPermeabilidadeLET : public ICurvasPermeabilidade {
private:
    // --- Parâmetros da Fase Aquosa ---
    double _Lw; ///< Parâmetro empírico de forma (L) para a água.
    double _Ew; ///< Parâmetro empírico de elevação (E) para a água.
    double _Tw; ///< Parâmetro empírico de translação (T) para a água.

    // --- Parâmetros da Fase Oleica ---
    double _Lo; ///< Parâmetro empírico de forma (L) para o óleo.
    double _Eo; ///< Parâmetro empírico de elevação (E) para o óleo.
    double _To; ///< Parâmetro empírico de translação (T) para o óleo.

    // --- Saturações de Contorno ---
    double _Swirr; ///< Saturação irredutível de água ($S_{wirr}$).
    double _Sor;   ///< Saturação residual de óleo ($S_{or}$).

public:
    /**
     * @brief Construtor principal parametrizado da classe LET.
     *
     * @param Lw Parâmetro L para água.
     * @param Ew Parâmetro E para água.
     * @param Tw Parâmetro T para água.
     * @param Lo Parâmetro L para óleo.
     * @param Eo Parâmetro E para óleo.
     * @param To Parâmetro T para óleo.
     * @param Swirr Saturação irredutível de água.
     * @param Sor Saturação residual de óleo.
     */
    CCurvasPermeabilidadeLET(double Lw, double Ew, double Tw,
                             double Lo, double Eo, double To,
                             double Swirr, double Sor);

    /**
     * @brief Construtor padrão.
     * Inicializa todos os coeficientes empíricos e saturações extremas com zero.
     */
    CCurvasPermeabilidadeLET();

    /**
     * @brief Destrutor virtual da classe.
     */
    virtual ~CCurvasPermeabilidadeLET();

    /**
     * @brief Carrega os coeficientes estruturais do modelo LET a partir de um arquivo de disco.
     *
     * A leitura espera a extração sequencial de 8 parâmetros no formato ASCII:
     * `[Lw] [Ew] [Tw] [Lo] [Eo] [To] [Swirr] [Sor]`
     *
     * @param arquivo Caminho relativo ou absoluto do arquivo de configuração (.txt).
     * @throws std::runtime_error Se o arquivo for inválido ou não contiver os 8 parâmetros.
     */
    void carregarDados(const std::string& arquivo) override;

    /**
     * @brief Computa a permeabilidade relativa da fase aquosa ($k_{rw}$) usando a formulação LET.
     * @param Sw Saturação atual de água ($S_w$).
     * @return O valor adimensional de $k_{rw}$.
     */
    double getKrw(double Sw) const override;

    /**
     * @brief Computa a permeabilidade relativa da fase oleica ($k_{ro}$) usando a formulação LET.
     * @param Sw Saturação atual de água ($S_w$).
     * @return O valor adimensional de $k_{ro}$.
     */
    double getKro(double Sw) const override;

    /** @brief Retorna a Saturação Irredutível. */
    double getSwi() const { return _Swirr; }

    /** @brief Retorna a Saturação de óleo residual. */
    double getSor() const { return _Sor; }
};

#endif // CCURVASPERMEABILIDADELET_H