#ifndef CCALCULADORAFLUXOFRACIONARIO_H
#define CCALCULADORAFLUXOFRACIONARIO_H

#include "ICurvasPermeabilidade.h"
#include <map>
#include <cmath>

/**
 * @brief Responsável pelos cálculos pontuais de fluxo fracionário e suas derivadas.
 *
 * Esta classe implementa a equação constitutiva do fluxo fracionário (Buckley-Leverett
 * generalizado), considerando forças viscosas e gravitacionais.
 *
 * A equação implementada é:
 * \f$ f_w = \frac{1 - \frac{k k_{ro}}{u_t \mu_o} \Delta\rho g \sin(\alpha)}{1 + \frac{k_{ro}}{k_{rw}} \frac{\mu_w}{\mu_o}} \f$
 *
 * @author Fabiane Barros
 * @date Janeiro 2026
 */
class CCalculadoraFluxoFracionario {
private:
    // Propriedades dos Fluidos
    double _mi_o;   ///< Viscosidade do óleo (cP ou Pa.s).
    double _mi_w;   ///< Viscosidade da água (cP ou Pa.s).
    double _rho_o;  ///< Densidade do óleo.
    double _rho_w;  ///< Densidade da água.

    // Propriedades do Reservatório/Fluxo
    double _k;      ///< Permeabilidade absoluta (mD ou m²).
    double _ut;     ///< Velocidade total de Darcy (m/s ou ft/day).
    double _angulo; ///< Ângulo de inclinação (radianos).
    double _g;      ///< Aceleração da gravidade.

    // Estratégia de Permeabilidade
    ICurvasPermeabilidade* _modeloKr; ///< Ponteiro para o modelo de permeabilidade relativa.

public:
    /**
     * @brief Construtor da calculadora.
     *
     * @param mu_o Viscosidade do óleo.
     * @param mu_w Viscosidade da água.
     * @param modelo Ponteiro para o modelo de curva de permeabilidade.
     * @param g Aceleração da gravidade (padrão = 9.81).
     */
    CCalculadoraFluxoFracionario(double mu_o, double mu_w, ICurvasPermeabilidade* modelo, double g = 9.80665);

    /**
     * @brief Atualiza todas as propriedades físicas necessárias para o cálculo.
     *
     * Método auxiliar para configurar a simulação de uma única vez.
     *
     * @param mi_w Viscosidade da água.
     * @param mi_o Viscosidade do óleo.
     * @param rho_w Densidade da água.
     * @param rho_o Densidade do óleo.
     * @param k Permeabilidade absoluta.
     * @param angulo Ângulo de inclinação (em graus, será convertido para radianos internamente).
     * @param ut Velocidade total de injeção.
     */
    void setPropriedades(double mi_w, double mi_o, double rho_w, double rho_o, double k, double angulo, double ut);

    /**
     * @brief Define o modelo de permeabilidade relativa a ser usado (Strategy).
     * @param modelo Ponteiro para a nova estratégia.
     */
    void setModeloPermeabilidade(ICurvasPermeabilidade* modelo);


    double calcularRapoportLeas(double L, double phi, double sigma) const;

    /** @brief Calcula a Razão de Mobilidade (M) para uma saturação dada */
    double calcularM(double sw) const;

    /** @brief Calcula o Número de Gravidade (Ng) para uma saturação dada */
    double calcularNg(double sw) const;

    /**
     * @brief Calcula o fluxo fracionário de água (fw) para uma dada saturação.
     *
     * Aplica a equação generalizada considerando mobilidade e gravidade.
     *
     * @param sw Saturação de água.
     * @return Valor adimensional de fw.
     */
    double calcularFw(double sw) const;

    /**
     * @brief Calcula a derivada do fluxo fracionário (dfw/dSw) numericamente.
     *
     * Utiliza o método das diferenças finitas centrais. Fundamental para
     * determinar a velocidade de avanço da frente de saturação no MOC.
     *
     * @param sw Saturação de água.
     * @return Valor da derivada (velocidade adimensional da onda).
     */
    double calcularDerivadaFw(double sw) const;

    /**
     * @brief Gera um mapa de pontos (Sw, fw) para plotagem.
     * @param passo Incremento de saturação (ex: 0.01).
     * @return Mapa contendo a curva completa.
     */
    std::map<double, double> gerarCurvaCompleta(double passo) const;
};

#endif
