#ifndef CSIMULADOR_H
#define CSIMULADOR_H

#include "ICurvasPermeabilidade.h"
#include "CCalculadoraFluxoFracionario.h"
#include "CSolver.h"
#include "CWelge.h"
#include "CMalha.h"
#include <string>

/**
 * @brief Controlador Principal do Sistema (Padrão Controller).
 *
 * Esta classe atua como a interface entre a Camada de Apresentação (MainWindow)
 * e a Camada de Domínio (Solver, Calculadora, Welge).
 *
 * Responsabilidades:
 * 1. Armazenar os parâmetros de entrada do usuário.
 * 2. Gerenciar o ciclo de vida dos objetos matemáticos (Solver, Malha, etc).
 * 3. Orquestrar a execução da simulação.
 *
 * @author Fabiane
 * @date Janeiro 2026
 */
class CSimulador {
private:
    // --- Objetos do Núcleo Matemático (Composição) ---
    CSolver* _solver;
    CCalculadoraFluxoFracionario* _calculadora;
    CWelge* _welge;
    CMalha* _malha;
    ICurvasPermeabilidade* _modeloPermeabilidade; // Ponteiro para a estratégia atual

    // --- Parâmetros Físicos do Reservatório ---
    double _comprimento;    ///< Comprimento do sistema (L) [m].
    double _area;           ///< Área de seção transversal (A) [m²].
    double _porosidade;     ///< Porosidade da rocha (phi) [fracao].
    double _angulo;         ///< Ângulo de inclinação [graus].
    double _vazaoInjecao;   ///< Vazão de injeção (Q) [m³/s].

    // --- Propriedades dos Fluidos ---
    double _mi_o;           ///< Viscosidade do óleo [cP].
    double _mi_w;           ///< Viscosidade da água [cP].
    double _rho_o;          ///< Densidade do óleo [kg/m³].
    double _rho_w;          ///< Densidade da água [kg/m³].

    // --- Propriedade da Rocha ---
    double _k;              ///< Permeabilidade absoluta [mD ou m²].

public:
    /**
     * @brief Construtor padrão.
     * Inicializa os ponteiros e define valores padrão seguros.
     */
    CSimulador();

    /**
     * @brief Destrutor.
     * Libera a memória de todos os objetos alocados.
     */
    ~CSimulador();

    // --- Configuração de Dados (Setters) ---

    /**
     * @brief Define os dados geométricos e petrofísicos do reservatório.
     */
    void setDadosReservatorio(double L, double A, double phi, double angulo, double vazao);

    /**
     * @brief Define as propriedades dos fluidos.
     */
    void setFluidos(double mi_o, double mi_w, double rho_o, double rho_w);

    /**
     * @brief Define a permeabilidade absoluta do reservatório.
     * @param k Permeabilidade absoluta [mD ou m²].
     */
    void setPermeabilidade(double k);

    /**
     * @brief Define a estratégia de permeabilidade a ser usada (Corey, LET, etc).
     * O simulador assume a posse do ponteiro e irá deletá-lo quando necessário.
     * @param modelo Ponteiro para a nova estratégia.
     */
    void setModeloPermeabilidade(ICurvasPermeabilidade* modelo);

    // --- Execução ---

    /**
     * @brief Executa a simulação completa para um determinado tempo.
     *
     * Fluxo de execução:
     * 1. Configura a Calculadora com os dados atuais.
     * 2. Configura o Solver.
     * 3. Chama o método de cálculo analítico do Solver.
     *
     * @param tempoInjetado Tempo em horas .
     */
    void executarSimulacao(double tempoInjetado, double qt,double A);

    // --- Acesso aos Resultados (Getters) ---

    CMalha* getMalha() const;
    CWelge* getWelge() const;
    CCalculadoraFluxoFracionario* getCalculadora() const;
    ICurvasPermeabilidade* getModeloPermeabilidade() const;
    double getComprimento() const;
    double getPorosidade() const;
};

#endif
