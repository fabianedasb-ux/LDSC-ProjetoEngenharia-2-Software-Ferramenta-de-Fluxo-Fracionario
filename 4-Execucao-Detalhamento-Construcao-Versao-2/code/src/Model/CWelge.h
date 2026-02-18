#ifndef CWELGE_H
#define CWELGE_H

#include "CCalculadoraFluxoFracionario.h"

/**
 * @brief Implementa o método da Tangente de Welge.
 *
 * Responsável por determinar a saturação da frente de choque (Sw_f) e a saturação
 * média (Sw_media) atrás da frente.
 *
 * Teoria:
 * A solução de Buckley-Leverett gera saturações multivaloradas (perfil em S)
 * fisicamente impossíveis. A construção de Welge impõe uma descontinuidade (choque)
 * que satisfaz a condição de entropia e o balanço de massa.
 *
 * Geometricamente, a saturação do choque é encontrada traçando uma reta tangente
 * à curva de fluxo fracionário partindo da saturação inicial.
 *
 * @author Fabiane Barros
 * @date Janeiro 2026
 */
class CWelge {
private:
    double _swFrente;      ///< Saturação na frente de choque (resultado principal).
    double _swMedia;       ///< Saturação média atrás da frente (útil para recuperação).
    double _swInicial;     ///< Saturação inicial de água no reservatório (Swir).
    double _inclinacaoMax; ///< Armazena o valor da inclinação da reta tangente.
    // Parâmetros de operação fornecidos pela UI/Simulador
    double _vazaoInjecao;  ///< Vazão de injeção (m3/s ou unidade definida pelo sistema).
    double _area;          ///< Área da seção transversal (m2).

public:
    /**
     * @brief Construtor padrão.
     */
    CWelge();

    /**
     * @brief Destrutor.
     */
    ~CWelge();

    /**
     * @brief Define a saturação inicial do reservatório.
     * @param swi Geralmente igual à saturação irredutível (Swir).
     */
    void setSwInicial(double swi);
    void setvazaoInjecao(double vazaoInjecao);
    void setarea(double area);

    /**
     * @brief Executa o algoritmo de busca da tangente.
     *
     * Varre a curva de fluxo fracionário gerada pela calculadora e encontra
     * o ponto que maximiza a inclinação da reta secante saindo de (Sw_inicial, fw_inicial).
     *
     * @param calc Ponteiro para a calculadora física (fornece fw).
     * @return true se encontrou um choque válido, false caso contrário.
     */
    bool calcularTangente(CCalculadoraFluxoFracionario* calc);

    // --- Getters ---

    /**
     * @brief Retorna a saturação da frente de choque encontrada.
     */
    double getSwFrente() const;

    /**
     * @brief Retorna a saturação média da zona varrida.
     * Calculada extrapolando a tangente até fw=1.
     */
    double getSwMedia() const;

    /**
     * @brief Retorna a velocidade (inclinação) do choque.
     */
    double getInclinacaoChoque() const;
};

#endif
