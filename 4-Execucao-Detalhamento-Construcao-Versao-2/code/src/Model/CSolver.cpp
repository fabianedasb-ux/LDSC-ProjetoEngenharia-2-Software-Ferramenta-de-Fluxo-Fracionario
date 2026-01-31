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
void CSolver::calcularPerfilSaturacao(double tempoInjetado) {
    if (!_malha || !_calc || !_welge) return;

    // 1. Reset da Simulação
    _malha->limpar();
    _malha->setTempoAtual(tempoInjetado); // t_D (PVI) [cite: 520, 562]

    // 2. Cálculo da Frente de Choque (Welge / Rankine-Hugoniot)
    // Determina a saturação Swf que encerra a zona de rarefação [cite: 592, 608]
    bool choqueEncontrado = _welge->calcularTangente(_calc);
    double swFrente = _welge->getSwFrente();
    double velocidadeChoque = _welge->getInclinacaoChoque(); // v_sigma = f'(Swf) [cite: 602]

    // 3. Construção do Perfil via MOC (Método das Características)
    // Substituição do loop double por inteiro para evitar warnings de precisão
    double passoSw = 0.005;
    int numPontos = static_cast<int>(1.0 / passoSw);

    for (int i = 0; i <= numPontos; ++i) {
        double sw = i * passoSw;
        if (sw > 1.0) sw = 1.0;

        double velocidadeOnda = 0.0;

        // --- Aplicação da Condição de Entropia ---
        // Saturações menores que a frente (Sw < Swf) são instáveis e "caem" no choque.
        // Elas viajam à velocidade da frente, criando a descontinuidade vertical[cite: 588, 592].
        if (choqueEncontrado && sw < swFrente) {
            velocidadeOnda = velocidadeChoque;
        }
        else {
            // Região de Rarefação: Sw >= Swf
            // Cada saturação viaja à sua própria velocidade característica f'(Sw)[cite: 560, 590].
            velocidadeOnda = _calc->calcularDerivadaFw(sw);
        }

        // --- Cálculo da Posição Adimensional (xD = v * tD) ---
        // Aqui tempoInjetado deve ser tD (Volume de Poros Injetados - PVI)[cite: 519, 562].
        double posicaoX = velocidadeOnda * tempoInjetado;

        // Adiciona o ponto à malha para visualização
        _malha->adicionarCelula(CCelula(sw, posicaoX, velocidadeOnda));
    }

    // 4. Estabilização do Perfil
    // Ordenar por posição garante que o gráfico desenhe a frente corretamente,
    // especialmente se houver efeitos de gravidade que alterem a monotonicidade[cite: 224, 442].
    _malha->ordenarPorPosicao();
}
