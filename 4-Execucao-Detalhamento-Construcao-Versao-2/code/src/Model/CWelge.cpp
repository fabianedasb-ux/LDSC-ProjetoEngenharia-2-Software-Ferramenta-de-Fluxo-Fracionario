/**
 * @file CWelge.cpp
 * @brief Implementação do algoritmo de construção da tangente de Welge.
 */

#include "CWelge.h"
#include <cmath>
#include <algorithm> // Para std::max

// --- Construtor ---
CWelge::CWelge()
    : _swFrente(0.0), _swMedia(0.0), _swInicial(0.0), _inclinacaoMax(0.0)
{
}

// --- Destrutor ---
CWelge::~CWelge() {
}

// --- Setter ---
void CWelge::setSwInicial(double swi) {
    _swInicial = swi;
}

// --- Algoritmo Principal ---
bool CWelge::calcularTangente(CCalculadoraFluxoFracionario* calc) {
    if (!calc) return false;

    // 1. Obter o ponto de partida (Sw_i, fw_i) [cite: 604, 609]
    double fw_inicial = calc->calcularFw(_swInicial);

    // Resetar valores para busca
    _inclinacaoMax = -1.0;
    _swFrente = _swInicial;

    // 2. Busca da Máxima Inclinação (Rankine-Hugoniot) [cite: 600, 602, 609]
    // Substituindo o loop de double por um contador inteiro para evitar warnings
    double passo = 0.001;
    int numPassos = static_cast<int>((1.0 - _swInicial) / passo);

    for (int i = 1; i <= numPassos; ++i) {
        double sw = _swInicial + (i * passo);
        if (sw > 1.0) sw = 1.0;

        double fw = calc->calcularFw(sw);
        double deltaSw = sw - _swInicial;

        // Condição de Rankine-Hugoniot: v_sigma = (fw - fwi) / (Sw - Swi) [cite: 600, 602]
        double inclinacaoSecante = (fw - fw_inicial) / deltaSw;

        // A tangente de Welge é o ponto de máxima inclinação da secante [cite: 609, 610]
        if (inclinacaoSecante > _inclinacaoMax) {
            _inclinacaoMax = inclinacaoSecante;
            _swFrente = sw;
        }
    }

    // 3. Validação da Condição de Entropia [cite: 580, 581, 585]
    // Se a inclinação máxima for a derivada no ponto inicial, não há choque (fluxo dispersivo)
    double derivadaInicial = calc->calcularDerivadaFw(_swInicial);
    if (_inclinacaoMax <= derivadaInicial + 1e-5) {
        _swFrente = _swInicial;
        _inclinacaoMax = derivadaInicial;
    }

    // 4. Cálculo da Saturação Média (Extrapolação da Tangente) [cite: 615, 616]
    // Eq: Sw_media = Sw_frente + (1 - fw_frente) / f'(Sw_frente)
    double fw_frente = calc->calcularFw(_swFrente);

    if (_inclinacaoMax > 1e-9) {
        _swMedia = _swFrente + (1.0 - fw_frente) / _inclinacaoMax;
    } else {
        _swMedia = _swFrente;
    }

    _swMedia = std::clamp(_swMedia, _swFrente, 1.0); // Restrição física [cite: 629]

    return true;
}

// --- Getters ---

double CWelge::getSwFrente() const {
    return _swFrente;
}

double CWelge::getSwMedia() const {
    return _swMedia;
}

double CWelge::getInclinacaoChoque() const {
    return _inclinacaoMax;
}
