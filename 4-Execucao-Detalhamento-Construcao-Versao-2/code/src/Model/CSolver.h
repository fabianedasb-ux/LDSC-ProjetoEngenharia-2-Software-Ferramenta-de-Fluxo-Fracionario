#ifndef CSOLVER_H
#define CSOLVER_H

/**
 * @file CSolver.h
 * @brief Definição da classe CSolver, motor principal da solução de Buckley-Leverett.
 */

#include "CMalha.h"
#include "CCalculadoraFluxoFracionario.h"
#include "CWelge.h" // Dependência necessária para correção da frente
#include <vector>

/**
 * @class CSolver
 * @brief Motor matemático do simulador de deslocamento imiscível 1D (Solução Analítica).
 *
 * A classe CSolver é a entidade coordenadora que aplica o Método das Características (MOC)
 * para solucionar a Equação a Derivadas Parciais (EDP) não-linear hiperbólica de Buckley-Leverett.
 *
 * Diferentemente da modelagem por simulação numérica discreta (Diferenças Finitas),
 * o solver analítico não avança a solução por passos de tempo de forma iterativa ao longo de uma malha fixa.
 * Em vez disso, dada uma saturação de água $S_w$, ele mapeia diretamente a propagação de sua característica
 * ao longo do domínio espacial, calculando o perfil instantâneo para o tempo injetado $t_D$ usando a relação cinemática:
 * \f$ x_D(S_w, t_D) = \left( \frac{df_w}{dS_w} \right) \cdot t_D \f$
 *
 * Adicionalmente, o solver orquestra a aplicação da condição de entropia macroscópica por meio
 * do algoritmo de Welge, garantindo o balanço de massa ao resolver a descontinuidade (frente de choque)
 * que previne soluções fisicamente impossíveis de múltipla valoração de saturação espacial.
 *
 * @author Fabiane da Silva Barros
 * @date Janeiro 2026
 */
class CSolver {
private:
    CMalha* _malha;                      ///< Ponteiro para a estrutura de dados onde o perfil temporal espacial será registrado.
    CCalculadoraFluxoFracionario* _calc; ///< Ponteiro associado à máquina de cálculo de estado termodinâmico e derivativo.
    CWelge* _welge;                      ///< Ponteiro para a interface geométrica do método de Welge e construção do choque de Buckley-Leverett.

public:
    /**
     * @brief Construtor padrão da classe CSolver.
     * Aloca referências de ponteiros como nulas, exigindo injeção de dependência explícita pela camada superior.
     */
    CSolver();

    /**
     * @brief Destrutor da classe.
     * A classe opera sob o conceito de agregação forte de ponteiros externos,
     * logo, o destrutor não executa a deleção física de `_malha`, `_calc` e `_welge` na memória heap.
     */
    ~CSolver();

    // --- Injeção de Dependências Analíticas ---

    /**
     * @brief Atrela uma malha topológica ao Solver.
     * @param malha Ponteiro ativo contendo um objeto tipo CMalha.
     */
    void setMalha(CMalha* malha);

    /**
     * @brief Determina o motor matemático base.
     * @param calc Ponteiro ativo contendo uma CCalculadoraFluxoFracionario instanciada.
     */
    void setCalculadora(CCalculadoraFluxoFracionario* calc);

    /**
     * @brief Define o resolvedor da singularidade frontal de Buckley-Leverett.
     * @param welge Ponteiro ativo da técnica construtiva de tangente de Welge.
     */
    void setWelge(CWelge* welge);

    // --- Núcleo Computacional ---

    /**
     * @brief Computa e monta todo o campo espacial de Saturação de Água vs Posição Adimensional no reservatório.
     *
     * O algoritmo interno procede da seguinte forma:
     * 1. Limpa os instantes passados armazenados da CMalha.
     * 2. Varre linearmente o domínio de Saturações possíveis da injeção.
     * 3. Projeta a velocidade $v(S_w) = f_w'$ e determina a distância propagada (Método das Características Analítico).
     * 4. Valida a instabilidade no contorno do perfil utilizando a técnica auxiliar CWelge (Tangente/Entropia).
     * 5. Corrige numericamente as velocidades irreais que causariam sobreposição da onda de avanço.
     * 6. Registra ordenadamente cada CCelula resultante.
     *
     * @param tempoInjetado Escala temporal (Adimensionalizada em PVI ou volumes porosos).
     * @param swi Saturação base (Irredutível de água) presente originalmente no reservatório, para restrição construtiva do Choque Welge.
     * @param sw_max Teto da onda aquosa (1 - Saturação residual oleica limitadora).
     */
    void calcularPerfilSaturacao(double tempoInjetado, double swi, double sw_max);
};

#endif // CSOLVER_H