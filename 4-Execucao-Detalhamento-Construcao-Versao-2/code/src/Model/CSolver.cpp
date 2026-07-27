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

    // Passo 2: Verificação da Condição de Entropia de Oleinik
    // Uma onda contínua é válida se a velocidade (derivada) for monotonicamente decrescente.
    bool necessitaWelge = false;

    // Varredura para encontrar violação da entropia (inversão de velocidades) no domínio móvel
    for (size_t i = 1; i < vecSw.size(); ++i) {
        if (vecSw[i] > swi && vecSw[i] < sw_max) {
            // Se a velocidade da saturação MAIOR for MAIOR que a da saturação MENOR:
            // Ocorre "atropelamento" de ondas (multivaloração irreal). A entropia falhou.
            if (vecVel[i] > vecVel[i - 1]) {
                necessitaWelge = true;
                break;
            }
        }
    }

    bool choqueEncontrado = false;
    double swFrente = swi;
    double velocidadeChoque = 0.0;

    // Árvore de Decisão Metodológica
    if (necessitaWelge) {
        // Solução de Choque: A entropia foi violada, invoca-se a tangente de Welge
        choqueEncontrado = _welge->calcularTangente(_calc, swi, sw_max);
        if (choqueEncontrado) {
            swFrente = _welge->getSwFrente();
            velocidadeChoque = _welge->getInclinacaoChoque();
        }
    } else {
        // Solução Clássica Contínua: Não há choque (Onda de Rarefação Pura)
        // A frente de avanço é ditada puramente pela maior velocidade natural (no caso, a base da curva)
        choqueEncontrado = false;
        swFrente = swi;
        velocidadeChoque = _calc->calcularDerivadaFw(swi);
    }

    // Passo 3 e 4: Construção Exata do Perfil Analítico
    double pos_choque = 0.0;
    if (choqueEncontrado) {
        pos_choque = velocidadeChoque * tempoInjetado;
    }

    for (size_t i = 0; i < vecSw.size(); ++i) {
        double sw = vecSw[i];
        double vel = vecVel[i];
        double pos = vel * tempoInjetado;

        // Ignora valores fora da janela de injeção física
        if (sw < swi || sw > sw_max) continue;

        if (choqueEncontrado) {
            // Plota APENAS a onda de rarefação (saturações maiores que a frente)
            if (sw > swFrente) {
                if (pos > pos_choque) pos = pos_choque; // Trava de segurança
                _malha->adicionarCelula(CCelula(sw, pos, vel));
            }
        } else {
            // Deslocamento contínuo sem choque
            _malha->adicionarCelula(CCelula(sw, pos, vel));
        }
    }

    // Passo 5: A Injeção da Descontinuidade (O DEGRAU)
    if (choqueEncontrado) {
        // Cravamos o topo e a base EXATAMENTE na mesma coordenada X
        _malha->adicionarCelula(CCelula(swFrente, pos_choque, velocidadeChoque));
        _malha->adicionarCelula(CCelula(swi, pos_choque, velocidadeChoque));
    }

    // Fecha o reservatório com a saturação inicial intocada
    _malha->adicionarCelula(CCelula(swi, 1.0, 0.0));

    _malha->adicionarCelula(CCelula(sw_max, 0.0, 0.0));

    // Passo 6: Ordenação da malha
    _malha->ordenarPorPosicao();
} // Fim da função calcularPerfilSaturacao