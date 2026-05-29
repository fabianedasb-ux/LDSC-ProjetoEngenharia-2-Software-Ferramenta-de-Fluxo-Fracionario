#ifndef CWELGE_H
#define CWELGE_H

/**
 * @file CWelge.h
 * @brief Definição da classe CWelge e da construção geométrica da frente de choque.
 */

#include "CCalculadoraFluxoFracionario.h"

/**
 * @class CWelge
 * @brief Implementa o método analítico da Tangente de Welge (1952).
 *
 * Responsável por determinar a saturação da frente de choque (\f$ S_{wf} \f$) e a saturação
 * média (\f$ \bar{S}_w \f$) da zona varrida atrás da frente de avanço.
 *
 * O Método das Características aplicado à equação de Buckley-Leverett pura gera
 * saturações multivaloradas (o chamado perfil "em S"), o que viola a conservação
 * de massa e o princípio da unicidade física. A construção de Welge impõe uma
 * descontinuidade brusca (choque) que satisfaz a condição de entropia de Oleinik.
 *
 * Geometricamente, o algoritmo localiza o ponto de tangência traçando retas secantes
 * a partir do ponto de saturação inicial \f$ (S_{wi}, f_w(S_{wi})) \f$ até atingir a inclinação máxima:
 * \f$ \left. \frac{df_w}{dS_w} \right|_{S_{wf}} = \frac{f_w(S_{wf}) - f_w(S_{wi})}{S_{wf} - S_{wi}} \f$
 *
 * @author Fabiane da Silva Barros
 * @date Janeiro 2026
 */
class CWelge {
private:
    double _swFrente;      ///< Saturação na frente de choque (\f$ S_{wf} \f$).
    double _swMedia;       ///< Saturação média da água atrás da frente geométrica (\f$ \bar{S}_w \f$).
    double _swInicial;     ///< Saturação inicial ou irredutível ancorada no reservatório (\f$ S_{wi} \f$).
    double _inclinacaoMax; ///< Máxima inclinação secante encontrada, equivalente à velocidade do choque.

    // Parâmetros operacionais sistêmicos
    double _vazaoInjecao;  ///< Vazão de injeção externa do fluido deslocante.
    double _area;          ///< Área de seção transversal do fluxo matricial.

public:
    /**
     * @brief Construtor padrão da classe CWelge.
     * Inicializa os atributos termodinâmicos e geométricos com valor nulo.
     */
    CWelge();

    /**
     * @brief Destrutor virtual da classe.
     */
    virtual ~CWelge();

    /**
     * @brief Define o ponto de ancoragem esquerdo da tangente.
     * @param swi Saturação inicial de água (\f$ S_{wi} \f$), usualmente igual a \f$ S_{wirr} \f$.
     */
    void setSwInicial(double swi);

    /**
     * @brief Executa o algoritmo de varredura analítica para a detecção da tangente de choque.
     *
     * O algoritmo varre discretamente a curva de fluxo fracionário gerada pela calculadora,
     * avaliando a derivada direcional da reta secante ancorada no ponto inicial.
     * O ponto que maximiza essa inclinação define a singularidade de choque.
     *
     * @param calc Ponteiro em execução para a calculadora física.
     * @param swi Saturação de água inicial fixada.
     * @param sw_max Teto da saturação de água (\f$ 1 - S_{or} \f$).
     * @return true se uma tangente com inclinação fisicamente válida (\f$ > 10^{-6} \f$) for encontrada.
     */
    bool calcularTangente(CCalculadoraFluxoFracionario* calc, double swi, double sw_max);

    // --- Getters ---

    /**
     * @brief Recupera a saturação exata onde ocorre a ruptura do perfil de avanço.
     * @return O valor de \f$ S_{wf} \f$.
     */
    double getSwFrente() const;

    /**
     * @brief Calcula e retorna a saturação média retida no meio poroso.
     * Extrapolação baseada na intersecção da tangente de choque com a assíntota \f$ f_w = 1 \f$.
     * @return O valor de \f$ \bar{S}_w \f$.
     */
    double getSwMedia() const;

    /**
     * @brief Recupera a inclinação da reta, que representa fisicamente a velocidade adimensional do choque.
     * @return O escalar \f$ \frac{df_w}{dS_w} \f$ avaliado no choque.
     */
    double getInclinacaoChoque() const;
};

#endif // CWELGE_H