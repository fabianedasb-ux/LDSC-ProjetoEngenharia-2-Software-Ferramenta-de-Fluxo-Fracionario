#ifndef CSIMULADOR_H
#define CSIMULADOR_H

/**
 * @file CSimulador.h
 * @brief Definição da classe CSimulador, o controlador central da arquitetura MVC.
 */

#include "ICurvasPermeabilidade.h"
#include "CCalculadoraFluxoFracionario.h"
#include "CSolver.h"
#include "CWelge.h"
#include "CMalha.h"
#include <string>

/**
 * @class CSimulador
 * @brief Controlador Principal do Sistema (Padrão Controller).
 *
 * Esta classe atua como a fachada unificada (Interface) entre a Camada de Apresentação (UI, MainWindow)
 * e o núcleo físico-matemático de Engenharia de Reservatórios.
 *
 * A sua responsabilidade primária é orquestrar o ciclo de vida e garantir a sincronização termodinâmica
 * antes de permitir a execução do Solver analítico.
 *
 * Funcionalidades principais:
 * 1. Armazenar os parâmetros de entrada operacionais, PVT e de rocha.
 * 2. Gerir rigidamente a memória alocada dos objetos em *heap* (Solver, Malha, Estratégia de Kr).
 * 3. Validação preliminar do critério de Rapoport-Leas.
 *
 * @author Fabiane da Silva Barros
 * @date Janeiro 2026
 */
class CSimulador {
private:
    // --- Objetos do Núcleo Matemático (Composição/Agregação) ---
    CSolver* _solver;                           ///< Ponteiro para o motor de resolução de EDPs (MOC).
    CCalculadoraFluxoFracionario* _calculadora; ///< Ponteiro para a calculadora do fluxo fracionário ($f_w$).
    CWelge* _welge;                             ///< Ponteiro para a lógica geométrica da onda de avanço.
    CMalha* _malha;                             ///< Ponteiro para a malha de estados espaciais.
    ICurvasPermeabilidade* _modeloPermeabilidade; ///< Ponteiro polimórfico (Strategy) em execução.

    // --- Parâmetros Físicos Estruturais ---
    double _comprimento;    ///< Comprimento total avaliado no reservatório ($L$) [m].
    double _area;           ///< Área de escoamento transversal ($A$) [m²].
    double _porosidade;     ///< Espaço poroso efetivo ($\phi$) [fração].
    double _angulo;         ///< Mergulho do estrato geológico [graus].
    double _vazaoInjecao;   ///< Vazão de injeção externa do fluido molhante ($Q_t$) [m³/s ou m³/d].

    // --- Propriedades PVT dos Fluidos ---
    double _mi_o;           ///< Viscosidade dinâmica da fase óleo ($\mu_o$) [cP].
    double _mi_w;           ///< Viscosidade dinâmica da fase água ($\mu_w$) [cP].
    double _rho_o;          ///< Densidade da fase óleo ($\rho_o$) [kg/m³].
    double _rho_w;          ///< Densidade da fase água ($\rho_w$) [kg/m³].

    // --- Propriedade Petrofísica Absoluta ---
    double _k;              ///< Permeabilidade escalar primária ($k$) [mD].

public:
    /**
     * @brief Construtor padrão da classe CSimulador.
     * Instancia o modelo empírico padrão (Corey) e inicializa toda a cascata de ponteiros matemáticos.
     */
    CSimulador();

    /**
     * @brief Destrutor virtual da classe.
     * Desaloca de forma segura a memória residual da malha, dos modelos e do resolvedor.
     */
    ~CSimulador();

    // --- Configuração e Validação de Dados (Setters) ---

    /**
     * @brief Injeta os dados geométricos e petrofísicos básicos do modelo geológico.
     * @param L Comprimento [m].
     * @param A Área [m²].
     * @param phi Porosidade efetiva.
     * @param angulo Inclinação do sistema gravitacional.
     * @param vazao Taxa total de injeção volumétrica.
     * @throws std::invalid_argument Se os parâmetros escalares essenciais forem nulos ou negativos.
     */
    void setDadosReservatorio(double L, double A, double phi, double angulo, double vazao);

    /**
     * @brief Injeta as propriedades de transporte dos fluidos bifásicos.
     * @param mi_o Viscosidade óleo.
     * @param mi_w Viscosidade água.
     * @param rho_o Densidade óleo.
     * @param rho_w Densidade água.
     */
    void setFluidos(double mi_o, double mi_w, double rho_o, double rho_w);

    /**
     * @brief Injeta a condutividade escalar principal da rocha.
     * @param k Permeabilidade absoluta [mD].
     */
    void setPermeabilidade(double k);

    /**
     * @brief Sobrescreve dinamicamente a estratégia de permeabilidade relativa (Padrão Strategy).
     * @note A gestão de memória passa a ser responsabilidade da classe. Ponteiros anteriores são destruídos.
     * @param modelo O novo ponteiro de política de permeabilidade instanciado (Ex: LET, Tabelado).
     */
    void setModeloPermeabilidade(ICurvasPermeabilidade* modelo);

    // --- Execução da Dinâmica ---

    /**
     * @brief Despoleta o motor analítico de simulação para um determinado volume de injeção.
     *
     * Este método propaga os parâmetros armazenados em interface para as calculadoras
     * de baixo nível e dispara o pipeline de cálculo espacial do Método das Características (MOC).
     *
     * @param tempoInjetado Tempo acumulado de injeção, tipicamente em volumes porosos.
     * @param swi Saturação de água residual de ancoragem inicial.
     * @param sw_max Teto da onda final pós-ruptura.
     */
    void executarSimulacao(double tempoInjetado, double swi, double sw_max);

    // --- Recuperação de Contextos (Getters) ---

    CMalha* getMalha() const;
    CWelge* getWelge() const;
    CCalculadoraFluxoFracionario* getCalculadora() const;
    ICurvasPermeabilidade* getModeloPermeabilidade() const;
    double getComprimento() const;
    double getPorosidade() const;
};

#endif // CSIMULADOR_H