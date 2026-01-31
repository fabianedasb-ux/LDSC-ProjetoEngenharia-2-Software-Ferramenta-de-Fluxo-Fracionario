/**
 * @file CSimulador.cpp
 * @brief Implementação do controlador CSimulador.
 */

#include "CSimulador.h"
#include "CCurvasPermeabilidadeCorey.h"
#include <iostream>
#include <stdexcept>

// --- Construtor ---
CSimulador::CSimulador()
    : _comprimento(100.0), _area(1.0), _porosidade(0.2), _angulo(0.0), _vazaoInjecao(1.0),
    _mi_o(1.0), _mi_w(1.0), _rho_o(800.0), _rho_w(1000.0)
{
    // 1. Criamos um modelo padrão (ex: Corey) primeiro
    _modeloPermeabilidade = new CCurvasPermeabilidadeCorey();

    // 2. Agora passamos esse modelo válido para a calculadora
    _calculadora = new CCalculadoraFluxoFracionario(_mi_o, _mi_w, _modeloPermeabilidade);

    _welge = new CWelge();
    _malha = new CMalha();
    _solver = new CSolver();

    _solver->setMalha(_malha);
    _solver->setCalculadora(_calculadora);
    _solver->setWelge(_welge);
}

// --- Destrutor ---
CSimulador::~CSimulador() {
    // Libera a memória na ordem inversa de dependência (boa prática)

    if (_solver) delete _solver;
    if (_malha) delete _malha;
    if (_welge) delete _welge;
    if (_calculadora) delete _calculadora;

    // O modelo de permeabilidade é deletado por último pois a calculadora o usa
    if (_modeloPermeabilidade) delete _modeloPermeabilidade;
}

// --- Setters de Dados ---

void CSimulador::setDadosReservatorio(double L, double A, double phi, double angulo, double vazao) {
    if (L <= 0 || A <= 0 || phi <= 0 || vazao <= 0) {
        throw std::invalid_argument("CSimulador: Parametros geometricos devem ser positivos.");
    }
    _comprimento = L;
    _area = A;
    _porosidade = phi;
    _angulo = angulo;
    _vazaoInjecao = vazao;
}

void CSimulador::setFluidos(double mi_o, double mi_w, double rho_o, double rho_w) {
    if (mi_o <= 0 || mi_w <= 0) {
        throw std::invalid_argument("CSimulador: Viscosidades devem ser positivas.");
    }
    _mi_o = mi_o;
    _mi_w = mi_w;
    _rho_o = rho_o;
    _rho_w = rho_w;
}

void CSimulador::setModeloPermeabilidade(ICurvasPermeabilidade* modelo) {
    if (modelo == nullptr) return;

    // Se já existia um modelo anterior, deleta para evitar vazamento de memória
    if (_modeloPermeabilidade != nullptr) {
        delete _modeloPermeabilidade;
    }

    _modeloPermeabilidade = modelo;

    // Atualiza a calculadora com a nova estratégia
    if (_calculadora) {
        _calculadora->setModeloPermeabilidade(_modeloPermeabilidade);
    }
}

// --- Execução ---

void CSimulador::executarSimulacao(double tempoInjetado) {
    if (!_modeloPermeabilidade) {
        throw std::runtime_error("CSimulador: Modelo de permeabilidade nao definido.");
    }

    // 1. Cálculo da Velocidade Total de Darcy (u_t)
    // ut = Q / A [m/s]
    double ut = _vazaoInjecao / _area;

    // 2. Validação Científica: Critério de Rapoport-Leas (N_RL)
    // De acordo com a Seção 3.2.5 (Eq. 3.9), precisamos de NRL >= 3 para Buckley-Leverett
    // Nota: A permeabilidade _k deve ser adicionada aos atributos do Simulador ou extraída da Rocha
    double k_absoluta = 1.0e-12; // Valor exemplo (1 Darcy em m²). Ideal: ler de _k.
    double tensao_interfacial = 0.03; // Exemplo [N/m]. Ideal: adicionar como atributo.

    double nrl = _calculadora->calcularRapoportLeas(_comprimento, _porosidade, tensao_interfacial);

    if (nrl < 3.0) {
        // Aqui você pode disparar um sinal para a GUI ou registrar um log de aviso
        std::cerr << "AVISO: Numero de Rapoport-Leas (" << nrl << ") < 3.0." << std::endl;
        std::cerr << "A interferencia da pressao capilar e significativa. Os resultados podem ser imprecisos." << std::endl;
    }

    // 3. Configuração da Calculadora
    // Passamos a velocidade de Darcy e o ângulo para o cálculo do Número de Gravidade (Ng) [cite: 440]
    _calculadora->setPropriedades(_mi_w, _mi_o, _rho_w, _rho_o, k_absoluta, _angulo, ut);

    // 4. Execução do Solver
    // O solver aplicará o MOC e a Condição de Entropia de Rankine-Hugoniot [cite: 538, 600]
    _solver->calcularPerfilSaturacao(tempoInjetado);
}

// --- Getters ---

CMalha* CSimulador::getMalha() const {
    return _malha;
}

CWelge* CSimulador::getWelge() const {
    return _welge;
}

CCalculadoraFluxoFracionario* CSimulador::getCalculadora() const {
    return _calculadora;
}

ICurvasPermeabilidade* CSimulador::getModeloPermeabilidade() const {
    return _modeloPermeabilidade;
}
