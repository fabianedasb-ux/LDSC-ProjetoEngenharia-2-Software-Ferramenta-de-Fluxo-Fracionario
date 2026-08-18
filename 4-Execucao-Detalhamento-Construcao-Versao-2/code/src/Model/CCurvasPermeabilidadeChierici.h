#ifndef CCURVASPERMEABILIDADECHIERICI_H
#define CCURVASPERMEABILIDADECHIERICI_H

/**
 * @file CCurvasPermeabilidadeChierici.h
 * @brief Definição da classe CCurvasPermeabilidadeChierici.
 */

#include "ICurvasPermeabilidade.h"
#include <cmath>
#include <string>

/**
 * @class CCurvasPermeabilidadeChierici
 * @brief Implementação empírica do modelo exponencial de permeabilidade relativa de Chierici (1984).
 *
 * A classe concretiza as equações propostas por Chierici para descrever o
 * escoamento bifásico no meio poroso. Diferentemente do modelo de Corey, o
 * modelo de Chierici baseia-se em funções exponenciais de decaimento, sendo
 * reconhecido pela elevada flexibilidade no ajuste da concavidade (curvature)
 * das curvas experimentais.
 *
 * As equações adotadas são:
 * - \f$ k_{rw} = k_{rw,max} \cdot \exp\left(-A_w \cdot S_{wn}^{-B_w}\right) \f$
 * - \f$ k_{ro} = k_{ro,max} \cdot \exp\left(-A_o \cdot (1 - S_{wn})^{-B_o}\right) \f$
 *
 * @author Fabiane da Silva Barros
 * @date Janeiro 2026
 */
class CCurvasPermeabilidadeChierici : public ICurvasPermeabilidade {
private:
    double _Aw;      ///< Parâmetro empírico exponencial A para a fase aquosa.
    double _Bw;      ///< Parâmetro empírico exponencial B para a fase aquosa.
    double _Ao;      ///< Parâmetro empírico exponencial A para a fase oleica.
    double _Bo;      ///< Parâmetro empírico exponencial B para a fase oleica.
    double _Swirr;   ///< Saturação irredutível de água ($S_{wirr}$).
    double _Sor;     ///< Saturação residual de óleo ($S_{or}$).
    double _kroMax;  ///< Ponto final (endpoint) da permeabilidade relativa do óleo.
    double _krwMax;  ///< Ponto final (endpoint) da permeabilidade relativa da água.

public:
    /**
     * @brief Construtor principal parametrizado do modelo de Chierici.
     *
     * @param Aw Parâmetro de decaimento (shape) para a água.
     * @param Bw Parâmetro de curvatura (curvature) para a água.
     * @param Ao Parâmetro de decaimento (shape) para o óleo.
     * @param Bo Parâmetro de curvatura (curvature) para o óleo.
     * @param Swirr Saturação irredutível de água.
     * @param Sor Saturação residual de óleo.
     * @param krwMax Valor escalar máximo admitido para $k_{rw}$.
     * @param kroMax Valor escalar máximo admitido para $k_{ro}$.
     */
    CCurvasPermeabilidadeChierici(double Aw, double Bw, double Ao, double Bo,
                                  double Swirr, double Sor,
                                  double krwMax, double kroMax);

    /**
     * @brief Construtor padrão.
     * Inicializa instâncias temporárias com coeficientes nulos em memória.
     */
    CCurvasPermeabilidadeChierici();

    /**
     * @brief Destrutor virtual da classe.
     */
    virtual ~CCurvasPermeabilidadeChierici();

    /**
     * @brief Carrega os coeficientes do modelo Chierici a partir de um arquivo em disco.
     *
     * Lê 8 parâmetros na seguinte ordem estrita:
     * `Aw Bw Ao Bo Swirr Sor KroMax KrwMax`
     *
     * @param arquivo Caminho para o arquivo `.txt`.
     * @throws std::runtime_error Em caso de falha de I/O ou incompatibilidade numérica.
     */
    void carregarDados(const std::string& arquivo) override;

    /**
     * @brief Computa a permeabilidade relativa contínua da água ($k_{rw}$).
     * @param Sw Saturação real da fase aquosa na célula analítica.
     * @return O escalar calculado de \f$ k_{rw} \f$.
     */
    double getKrw(double Sw) const override;

    /**
     * @brief Computa a permeabilidade relativa contínua do óleo ($k_{ro}$).
     * @param Sw Saturação real da fase aquosa na célula analítica.
     * @return O escalar calculado de \f$ k_{ro} \f$.
     */
    double getKro(double Sw) const override;

    /** @brief Retorna a Saturação Irredutível. */
    double getSwi() const { return _Swirr; }

    /** @brief Retorna a Saturação de óleo residual. */
    double getSor() const { return _Sor; }

    //bueno; documentar
    double getAw() { return _Aw; }
    double getBw() { return _Bw; }
    double getAo() { return _Ao; }
    double getBo() { return _Bo; }
    double getkroMax() { return _kroMax; }
    double getkrwMax() { return _krwMax; }
};
#endif // CCURVASPERMEABILIDADECHIERICI_H
