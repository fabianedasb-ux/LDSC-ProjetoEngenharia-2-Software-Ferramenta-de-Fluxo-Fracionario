#ifndef CCALCULADORAFLUXOFRACIONARIO_H
#define CCALCULADORAFLUXOFRACIONARIO_H

/**
 * @file CCalculadoraFluxoFracionario.h
 * @brief Definição da classe CCalculadoraFluxoFracionario.
 */

#include "ICurvasPermeabilidade.h"
#include <map>
#include <cmath>

/**
 * @class CCalculadoraFluxoFracionario
 * @brief Responsável pelos cálculos pontuais do fluxo fracionário e suas derivadas numéricas.
 *
 * Esta classe implementa a equação constitutiva do fluxo fracionário (Buckley-Leverett
 * generalizado), acoplando os efeitos de forças viscosas e gravitacionais no escoamento
 * bifásico incompressível.
 *
 * A equação matemática governante implementada é:
 * \f$ f_w = \frac{1 - \frac{k \cdot k_{ro}}{u_t \cdot \mu_o} \Delta\rho \cdot g \cdot \sin(\alpha)}{1 + \frac{k_{ro}}{k_{rw}} \frac{\mu_w}{\mu_o}} \f$
 *
 * Onde:
 * - \f$ f_w \f$ é a fração do fluxo total correspondente à água.
 * - \f$ \alpha \f$ é o ângulo de mergulho do reservatório.
 * - \f$ u_t \f$ é a velocidade total de Darcy.
 *
 * @author Fabiane da Silva Barros
 * @date Janeiro 2026
 */
class CCalculadoraFluxoFracionario {
private:
    // --- Propriedades PVT dos Fluidos ---
    double _mi_o;   ///< Viscosidade dinâmica da fase oleica (\f$ \mu_o \f$) [cP].
    double _mi_w;   ///< Viscosidade dinâmica da fase aquosa (\f$ \mu_w \f$) [cP].
    double _rho_o;  ///< Densidade da fase oleica (\f$ \rho_o \f$) [kg/m³].
    double _rho_w;  ///< Densidade da fase aquosa (\f$ \rho_w \f$) [kg/m³].

    // --- Propriedades Estruturais e Operacionais ---
    double _k;      ///< Permeabilidade absoluta do meio poroso (\f$ k \f$) [mD].
    double _qt;     ///< Vazão volumétrica de injeção total (\f$ q_t \f$) [m³/d].
    double _A;      ///< Área transversal hidráulica do reservatório (\f$ A \f$) [m²].
    double _ut;     ///< Velocidade total aparente de Darcy (\f$ u_t = q_t/A \f$) [m/d].
    double _angulo; ///< Ângulo de mergulho estrutural (\f$ \alpha \f$) [radianos].
    double _g;      ///< Aceleração gravitacional local (\f$ g \f$) [m/s²].

    // --- Acoplamento de Domínio (Padrão Strategy) ---
    ICurvasPermeabilidade* _modeloKr; ///< Ponteiro polimórfico para o modelo de permeabilidade relativa ativo.

public:
    /**
     * @brief Construtor padrão da calculadora física.
     *
     * @param mu_o Viscosidade do óleo.
     * @param mu_w Viscosidade da água.
     * @param modelo Ponteiro alocado para o modelo matemático de rocha-fluido.
     * @param g Constante de aceleração gravitacional (Default = 9.80665 m/s²).
     */
    CCalculadoraFluxoFracionario(double mu_o, double mu_w, ICurvasPermeabilidade* modelo, double g = 9.80665);

    /**
     * @brief Injeta e atualiza as propriedades físicas necessárias para o balanço de forças.
     *
     * @param mi_w Viscosidade da água [cP].
     * @param mi_o Viscosidade do óleo [cP].
     * @param rho_w Densidade da água [kg/m³].
     * @param rho_o Densidade do óleo [kg/m³].
     * @param k Permeabilidade absoluta [mD].
     * @param angulo Inclinação do estrato em graus (convertido para radianos internamente).
     * @param qt Vazão de injeção [m³/d].
     * @param A Área da seção [m²].
     */
    void setPropriedades(double mi_w, double mi_o, double rho_w, double rho_o, double k, double angulo, double qt, double A);

    /**
     * @brief Modifica dinamicamente a estratégia de permeabilidade relativa.
     * @param modelo Ponteiro para a nova interface ICurvasPermeabilidade.
     */
    void setModeloPermeabilidade(ICurvasPermeabilidade* modelo);

    // --- Diagnósticos Dimensionais e Adimensionais ---

    /**
     * @brief Avalia o critério macroscópico de estabilidade capilar (Rapoport-Leas).
     * \f$ N_{RL} = \left( \frac{\phi}{k} \right)^{1/2} \frac{L \cdot u_t \cdot \mu_w}{\sigma \cdot k_{rw}^0 \cdot \cos(\theta)} \f$
     * @param L Comprimento característico do sistema [m].
     * @param phi Porosidade efetiva da rocha [fração].
     * @param sigma Tensão interfacial água-óleo [mN/m ou dinas/cm].
     * @return O escalar do número de Rapoport-Leas.
     */
    double calcularRapoportLeas(double L, double phi, double sigma) const;

    /**
     * @brief Calcula a Razão de Mobilidade Global no Ponto Final (\f$ M^0 \f$).
     * \f$ M^0 = \frac{k_{rw}^0 / \mu_w}{k_{ro}^0 / \mu_o} \f$
     * @return Valor adimensional indicando a estabilidade do deslocamento pistonado.
     */
    double calcularM0() const;

    /**
     * @brief Calcula o Número de Gravidade no Ponto Final (\f$ N_g^0 \f$).
     * Analisa a razão entre as forças gravitacionais e as forças viscosas tratoras.
     * \f$ N_g^0 = \frac{k \cdot k_{ro}^0 \cdot \Delta\rho \cdot g}{u_t \cdot \mu_o} \f$
     * @return O escalar adimensional associado à segregação gravitacional.
     */
    double calcularNg0() const;

    // --- Resolução Numérica ---

    /**
     * @brief Computa a taxa de fluxo fracionário instantâneo (\f$ f_w \f$).
     * @param sw Saturação local de água no volume de controle.
     * @return Fração do fluxo, tipicamente entre 0 e 1.
     */
    double calcularFw(double sw) const;

    /**
     * @brief Avalia a velocidade adimensional da frente de avanço (\f$ f_w' \f$).
     * Utiliza o método de diferenças finitas centrais de alta ordem para evitar ruído.
     * @param sw Saturação local de água.
     * @return A derivada direcional \f$ \frac{df_w}{dS_w} \f$.
     */
    double calcularDerivadaFw(double sw);

    /**
     * @brief Discretiza a curva completa de fluxo fracionário.
     * @param passo Incremento de saturação (ex: $\Delta S_w = 0.01$).
     * @return Um dicionário (map) relacionando ordenadamente Saturações a Fluxos Fracionários.
     */
    std::map<double, double> gerarCurvaCompleta(double passo) const;
};

#endif // CCALCULADORAFLUXOFRACIONARIO_H