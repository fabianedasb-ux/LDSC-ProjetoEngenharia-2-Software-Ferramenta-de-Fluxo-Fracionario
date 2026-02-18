/**
 * @file CSolver.cpp
 * @brief Implementação da solução analítica de Buckley-Leverett.
 */

#include "CSolver.h"
#include <cmath>

// --- Construtor e Destrutor ---
CSolver::CSolver() : _malha(nullptr), _calc(nullptr), _welge(nullptr) {
}

CSolver::~CSolver() {
    // Não deletamos os ponteiros aqui pois eles são gerenciados pelo CSimulador (agregação)
}

// --- Setters ---
void CSolver::setMalha(CMalha* malha) {
    _malha = malha;
}

void CSolver::setCalculadora(CCalculadoraFluxoFracionario* calc) {
    _calc = calc;
}

void CSolver::setWelge(CWelge* welge) {
    _welge = welge;
}

// --- Motor Matemático (Lógica Principal) ---
void CSolver::calcularPerfilSaturacao(double tempoInjetado, double _vazaoInjecao, double _area) {
    if (!_malha || !_calc || !_welge) return;

    // Reset e preparação
    _malha->limpar();
    _malha->setTempoAtual(tempoInjetado); // t_D (PVI)


    // 1) Construir MOC: calcular velocidades características e posições para cada Sw
    double passoSw = 0.005;
    int numPontos = static_cast<int>(1.0 / passoSw);

    // Vetores temporários para armazenar Sw, velocidade característica e posição
    std::vector<double> vecSw;
    std::vector<double> vecVel;
    std::vector<double> vecPos;
    vecSw.reserve(numPontos + 1);
    vecVel.reserve(numPontos + 1);
    vecPos.reserve(numPontos + 1);

    for (int i = 0; i <= numPontos; ++i) {
        double sw = i * passoSw;
        if (sw > 1.0) sw = 1.0;
        double vel = _calc->calcularDerivadaFw(sw, _vazaoInjecao, _area); // velocidade característica f'(Sw)
        double pos = vel * tempoInjetado; // xD = v_c * tD

        vecSw.push_back(sw);
        vecVel.push_back(vel);
        vecPos.push_back(pos);
    }

    // 2) Determinar se existe choque aplicável (Welge) e obter Swf e velocidade do choque
    // Nota: calculamos MOC primeiro e em seguida verificamos a necessidade física do choque.
    bool choqueEncontrado = _welge->calcularTangente(_calc);
    double swFrente = _welge->getSwFrente();
    double velocidadeChoque = _welge->getInclinacaoChoque();

    // 3) Verificar condição de entropia / cruzamento de características
    // Simples heurística: se algum ponto com Sw < Swf possui posicao maior que
    // o ponto em Swf, então há cruzamento e o choque é relevante.
    bool cruzamento = false;
    double posSwFrente = 0.0;
    for (size_t i = 0; i < vecSw.size(); ++i) {
        if (vecSw[i] >= swFrente) {
            posSwFrente = vecPos[i];
            break;
        }
    }
    for (size_t i = 0; i < vecSw.size(); ++i) {
        if (vecSw[i] < swFrente && vecPos[i] > posSwFrente + 1e-12) {
            cruzamento = true;
            break;
        }
    }

    // 4) Preencher a malha: aplicar choque somente se ele for físico (cruzamento + choqueEncontrado)
    for (size_t i = 0; i < vecSw.size(); ++i) {
        double sw = vecSw[i];
        double pos = vecPos[i];
        double vel = vecVel[i];

        if (choqueEncontrado && cruzamento && sw < swFrente) {
            // Região colapsada: todas as saturações menores que Swf são representadas
            // pela posição do choque
            pos = velocidadeChoque * tempoInjetado;
            vel = velocidadeChoque;
        }

        _malha->adicionarCelula(CCelula(sw, pos, vel));
    }

    // Ordenar por posição para garantir plot contínuo
    _malha->ordenarPorPosicao();
}
