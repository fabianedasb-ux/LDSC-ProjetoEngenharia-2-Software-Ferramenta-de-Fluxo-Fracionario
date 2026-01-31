#ifndef CSOLVER_H
#define CSOLVER_H

#include "CMalha.h"
#include "CCalculadoraFluxoFracionario.h"
#include "CWelge.h" // Dependência necessária para correção da frente
#include <vector>

/**
 * @brief Motor matemático do simulador (Solução Analítica).
 *
 * Responsável por aplicar o Método das Características (MOC) para determinar
 * a posição da frente de saturação em um determinado tempo.
 *
 * Lógica Analítica:
 * Em vez de avançar pequenos passos de tempo (dt), este solver calcula
 * diretamente a posição x de cada saturação Sw para um tempo total t,
 * usando a derivada do fluxo fracionário.
 *
 * @author Fabiane Barros
 * @date Janeiro 2026
 */
class CSolver {
private:
    CMalha* _malha;              ///< Ponteiro para a malha onde os dados serão salvos.
    CCalculadoraFluxoFracionario* _calc; ///< Ponteiro para a calculadora física.
    CWelge* _welge;              ///< Ponteiro para o algoritmo de Welge (choque).

public:
    /**
     * @brief Construtor padrão.
     */
    CSolver();

    /**
     * @brief Destrutor.
     */
    ~CSolver();

    // --- Configuração ---

    /**
     * @brief Define a malha a ser populada.
     */
    void setMalha(CMalha* malha);

    /**
     * @brief Define a calculadora de física a ser usada.
     */
    void setCalculadora(CCalculadoraFluxoFracionario* calc);

    /**
     * @brief Define o objeto Welge para cálculo da frente de choque.
     */
    void setWelge(CWelge* welge);

    // --- Motor Matemático ---

    /**
     * @brief Calcula o perfil de saturação analítico para um tempo específico.
     *
     * Este é o método principal que implementa a lógica:
     * x = (dfw/dSw) * tempo.
     *
     * Também aplica a correção de Welge (Condição de Entropia) para garantir
     * que a frente de choque seja vertical (velocidade constante na frente).
     *
     * @param tempoInjetado Tempo adimensional (PVI) ou dimensional (dias).
     */
    void calcularPerfilSaturacao(double tempoInjetado);
};

#endif
