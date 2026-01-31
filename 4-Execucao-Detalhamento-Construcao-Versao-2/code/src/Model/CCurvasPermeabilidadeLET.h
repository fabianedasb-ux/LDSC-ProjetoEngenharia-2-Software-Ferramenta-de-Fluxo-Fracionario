#ifndef CCURVASPERMEABILIDADELET_H
#define CCURVASPERMEABILIDADELET_H

#include "ICurvasPermeabilidade.h"
#include <cmath>
#include <string>

/**
 * @brief Implementação do modelo de permeabilidade relativa LET.
 *
 * O modelo LET (Lombez, Escovedo, Trindade) é uma correlação empírica flexível
 * que utiliza três parâmetros (L, E, T) para descrever a forma, elevação e
 * translação da curva de permeabilidade relativa.
 *
 * Referência: Lombez et al. (2008).
 *
 * @author Fabiane Barros
 * @date Janeiro 2026
 */
class CCurvasPermeabilidadeLET : public ICurvasPermeabilidade {
private:
    // Parâmetros para a fase aquosa
    double _Lw; ///< Parâmetro de forma (L) para água.
    double _Ew; ///< Parâmetro de elevação (E) para água.
    double _Tw; ///< Parâmetro de translação (T) para água.

    // Parâmetros para a fase oleosa
    double _Lo; ///< Parâmetro de forma (L) para óleo.
    double _Eo; ///< Parâmetro de elevação (E) para óleo.
    double _To; ///< Parâmetro de translação (T) para óleo.

    // Saturações extremas
    double _Swirr; ///< Saturação irreduzível de água.
    double _Sor;   ///< Saturação residual de óleo.

public:
    /**
     * @brief Construtor principal da classe LET.
     *
     * @param Lw Parâmetro L para água.
     * @param Ew Parâmetro E para água.
     * @param Tw Parâmetro T para água.
     * @param Lo Parâmetro L para óleo.
     * @param Eo Parâmetro E para óleo.
     * @param To Parâmetro T para óleo.
     * @param Swirr Saturação irreduzível de água.
     * @param Sor Saturação residual de óleo.
     */
    CCurvasPermeabilidadeLET(double Lw, double Ew, double Tw,
                             double Lo, double Eo, double To,
                             double Swirr, double Sor);

    /**
     * @brief Construtor padrão (Vazio).
     * Inicializa todos os parâmetros com zero.
     */
    CCurvasPermeabilidadeLET();

    /**
     * @brief Destrutor virtual.
     */
    virtual ~CCurvasPermeabilidadeLET();

    /**
     * @brief Carrega dados de um arquivo externo.
     * @param arquivo Caminho para o arquivo.
     */
    void carregarDados(const std::string& arquivo) override;

    /**
     * @brief Calcula Krw usando a correlação LET.
     * Formula: Krw = (Swn^Lw) / (Swn^Lw + Ew * (1-Swn)^Tw)
     * @param Sw Saturação de água.
     * @return Valor de Krw.
     */
    double getKrw(double Sw) const override;

    /**
     * @brief Calcula Kro usando a correlação LET.
     * Formula: Kro = ((1-Swn)^Lo) / ((1-Swn)^Lo + Eo * Swn^To)
     * @param Sw Saturação de água.
     * @return Valor de Kro.
     */
    double getKro(double Sw) const override;
};

#endif
