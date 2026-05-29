/**
 * @file CSimulador.cpp
 * @brief Implementação da integração e gestão do fluxo MVC analítico.
 * @author Fabiane da Silva Barros
 * @date Janeiro 2026
 */

#include "CSimulador.h"
#include "CCurvasPermeabilidadeCorey.h"
#include <iostream>
#include <stdexcept>

// --- Construtor ---
CSimulador::CSimulador()
    : _comprimento(100.0), _area(1.0), _porosidade(0.2), _angulo(0.0), _vazaoInjecao(1.0),
    _mi_o(1.0), _mi_w(1.0), _rho_o(800.0), _rho_w(1000.0), _k(1.0e-12)
{
    // Construção inicial da dependência forte (modelo padrão)
    _modeloPermeabilidade = new CCurvasPermeabilidadeCorey();

    // Cascata de inicialização das instâncias dependentes
    _calculadora = new CCalculadoraFluxoFracionario(_mi_o, _mi_w, _modeloPermeabilidade);
    _welge = new CWelge();
    _malha = new CMalha();
    _solver = new CSolver();

    // Acoplamento da malha de comunicação do motor
    _solver->setMalha(_malha);
    _solver->setCalculadora(_calculadora);
    _solver->setWelge(_welge);
}

// --- Destrutor Seguro ---
CSimulador::~CSimulador() {
    // Destruição LIFO (Last-In-First-Out) para garantir segurança de ponteiros nulos
    if (_solver) delete _solver;
    if (_malha) delete _malha;
    if (_welge) delete _welge;
    if (_calculadora) delete _calculadora;

    // A política de interface de curvas só pode ser descartada no fim
    if (_modeloPermeabilidade) delete _modeloPermeabilidade;
}

// --- Delegação de Configurações ---

void CSimulador::setDadosReservatorio(double L, double A, double phi, double angulo, double vazao) {
    if (L <= 0 || A <= 0 || phi <= 0 || vazao <= 0) {
        throw std::invalid_argument("Exceção Simulador: Topologia espacial fisicamente inconsistente (valores nulos).");
    }
    _comprimento = L;
    _area = A;
    _porosidade = phi;
    _angulo = angulo;
    _vazaoInjecao = vazao;
}

double CSimulador::getComprimento() const {
    return _comprimento;
}

double CSimulador::getPorosidade() const {
    return _porosidade;
}

void CSimulador::setFluidos(double mi_o, double mi_w, double rho_o, double rho_w) {
    if (mi_o <= 0 || mi_w <= 0) {
        throw std::invalid_argument("Exceção Simulador: As propriedades termodinâmicas (Viscosidade) não podem ser nulas.");
    }
    _mi_o = mi_o;
    _mi_w = mi_w;
    _rho_o = rho_o;
    _rho_w = rho_w;
}

void CSimulador::setPermeabilidade(double k) {
    if (k <= 0) {
        throw std::invalid_argument("Exceção Simulador: Permeabilidade não convergente para o modelo.");
    }
    _k = k;
}

void CSimulador::setModeloPermeabilidade(ICurvasPermeabilidade* modelo) {
    if (modelo == nullptr) return;

    // Prevenção crítica contra vazamento de memória ram (Memory Leak)
    if (_modeloPermeabilidade != nullptr) {
        delete _modeloPermeabilidade;
    }

    _modeloPermeabilidade = modelo;

    // Repasse da alteração contextual à calculadora termodinâmica
    if (_calculadora) {
        _calculadora->setModeloPermeabilidade(_modeloPermeabilidade);
    }
}

// --- Chamada Principal de Execução ---

void CSimulador::executarSimulacao(double tempoInjetado, double swi, double sw_max) {
    if (!_modeloPermeabilidade) {
        throw std::runtime_error("Exceção Simulador: Impossível realizar varredura sem um algoritmo de curvas definido.");
    }

    const double tensao_interfacial = 0.03; // N/m típico para sistemas rocha-óleo-água

    // Check-sum analítico: O número de Rapoport-Leas avalia se as forças capilares
    // estabilizam a onda, ou se o deslocamento requer correção difusiva
    double nrl = _calculadora->calcularRapoportLeas(_comprimento, _porosidade, tensao_interfacial);
    if (nrl < 3.0) {
        std::cerr << "AVISO GEOLÓGICO: Número de Rapoport-Leas (" << nrl << ") < 3.0. A assunção de forças viscosas dominantes pode ser imprecisa neste reservatório." << std::endl;
    }

    // Sincroniza estado de massa e geometria para a GPU virtual
    _calculadora->setPropriedades(_mi_w, _mi_o, _rho_w, _rho_o, _k, _angulo, _vazaoInjecao, _area);

    // Delega os esforços resolutivos para a rotina abstrata
    _solver->calcularPerfilSaturacao(tempoInjetado, swi, sw_max);
}

// --- Getters Internos ---

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