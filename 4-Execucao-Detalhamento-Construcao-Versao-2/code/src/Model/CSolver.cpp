/**
 * @file CSolver.cpp
 * @brief Implementação analítica das leis de conservação pelo método das características.
 * @author Fabiane da Silva Barros
 * @date Janeiro 2026
 */

#include "CSolver.h"
#include <cmath>
#include <vector>

// --- Construtor e Destrutor ---
CSolver::CSolver() : _malha(nullptr), _calc(nullptr), _welge(nullptr) {
}

CSolver::~CSolver() {
    // Gestão de Ciclo de Vida: Deleção delegada ao CSimulador
}

// --- Setters de Estruturação ---
void CSolver::setMalha(CMalha* malha) {
    _malha = malha;
}

void CSolver::setCalculadora(CCalculadoraFluxoFracionario* calc) {
    _calc = calc;
}

void CSolver::setWelge(CWelge* welge) {
    _welge = welge;
}

// --- Algoritmo Base de MOC ---
void CSolver::calcularPerfilSaturacao(double tempoInjetado, double swi, double sw_max) {

    // Verificação de Seguridade Computacional: Checagem contra nullptr
    if (!_malha || !_calc || !_welge) return;

    _malha->limpar();
    _malha->setTempoAtual(tempoInjetado);

    // Resolução Fina Discreta Analítica
    double passoSw = 0.005;
    int numPontos = static_cast<int>(1.0 / passoSw);

    std::vector<double> vecSw;
    std::vector<double> vecVel;
    std::vector<double> vecPos;

    vecSw.reserve(numPontos + 1);
    vecVel.reserve(numPontos + 1);
    vecPos.reserve(numPontos + 1);

    // Passo 1: Construção do Perfil Bruto de Múltipla Valoração (Avanço Livre de Ondas)
    for (int i = 0; i <= numPontos; ++i) {
        double sw = i * passoSw;
        if (sw > 1.0) sw = 1.0;

        // A velocidade de cada saturação específica independe do tempo injetado na analítica
        double vel = _calc->calcularDerivadaFw(sw);

        // Posição adimensional da onda: (dfw / dSw) * tempo
        double pos = vel * tempoInjetado;

        vecSw.push_back(sw);
        vecVel.push_back(vel);
        vecPos.push_back(pos);
    }

    // Passo 2: Verificação do Front de Choque Instável
    // Avalia a necessidade termodinâmica do choque limitando ao espectro de móveis
    bool choqueEncontrado = _welge->calcularTangente(_calc, swi, sw_max);

    // Atributos definidores da frente estabilizada
    double swFrente = _welge->getSwFrente();
    double velocidadeChoque = _welge->getInclinacaoChoque();

    // Passo 3: Identificação Topológica da Dupla Valoração (Efeito "Quebra de Onda" de Choque)
    bool cruzamento = false;
    double posSwFrente = 0.0;

    for (size_t i = 0; i < vecSw.size(); ++i) {
        if (vecSw[i] >= swFrente) {
            posSwFrente = vecPos[i];
            break;
        }
    }

    // Checa se há porções do vetor que geometricamente estão a frente da linha crítica (sobreposição não-física)
    for (size_t i = 0; i < vecSw.size(); ++i) {
        if (vecSw[i] < swFrente && vecPos[i] > posSwFrente + 1e-12) {
            cruzamento = true;
            break;
        }
    }

    // Passo 4: Filtragem de Entropia - Remoção dos perfis lentos "comidos" pela onda frontal de Choque (Pistonamento ideal)
    for (size_t i = 0; i < vecSw.size(); ++i) {
        double sw = vecSw[i];
        double pos = vecPos[i];
        double vel = vecVel[i];

        if (choqueEncontrado && cruzamento && sw < swFrente) {
            pos = velocidadeChoque * tempoInjetado; // Fixa todas essas saturações lentas na parede vetorial do choque vertical
            vel = velocidadeChoque;
        }

        _malha->adicionarCelula(CCelula(sw, pos, vel));
    }

    // Passo 5: Restauração da Integridade do Plot Vectorial
    _malha->ordenarPorPosicao();
}