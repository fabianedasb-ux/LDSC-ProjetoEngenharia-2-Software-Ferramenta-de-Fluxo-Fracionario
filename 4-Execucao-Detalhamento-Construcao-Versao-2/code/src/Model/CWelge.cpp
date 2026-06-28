/**
 * @file CWelge.cpp
 * @brief Implementação dos cálculos de descontinuidade da frente de saturação.
 * @author Fabiane da Silva Barros
 * @date Janeiro 2026
 */

#include "CWelge.h"
#include <cmath>
#include <algorithm>

// --- Construtor ---
CWelge::CWelge()
    : _swFrente(0.0), _swMedia(0.0), _swInicial(0.0), _inclinacaoMax(0.0), _vazaoInjecao(0.0), _area(0.0)
{
}

// --- Destrutor ---
CWelge::~CWelge() {
}

// --- Setter de Condição Inicial ---
void CWelge::setSwInicial(double swi) {
    _swInicial = swi;
}

// --- Algoritmo Principal de Varredura ---
bool CWelge::calcularTangente(CCalculadoraFluxoFracionario* calc, double swi, double sw_max) {

    // Reset de segurança do estado geométrico
    _swInicial = swi;
    _inclinacaoMax = 0.0;
    _swFrente = swi;

    double fw_swi = calc->calcularFw(swi);

    // Varredura da secante ancorada fortemente em (Swi, fw(Swi))
    // Correção: Offset de 0.01 na origem da varredura elimina a captura de
    // assíntotas falsas geradas pela continuidade C0 da interpolação linear.
    for (double s = swi + 0.01; s <= sw_max; s += 0.001) {

        // Avaliação do coeficiente angular da secante
        double slope = (calc->calcularFw(s) - fw_swi) / (s - swi);

        // A condição de máximo garante o cumprimento da Condição de Entropia
        if (slope > _inclinacaoMax) {
            _inclinacaoMax = slope;
            _swFrente = s;
        }
    }

    // Validação da viabilidade física da injeção (inclinação não nula)
    if (_inclinacaoMax > 1e-6) {

        // O balanço de materiais de Welge permite encontrar a Saturação Média
        // através do prolongamento da tangente até o eixo fw = 1.0
        double fw_frente = calc->calcularFw(_swFrente);
        _swMedia = _swFrente + (1.0 - fw_frente) / _inclinacaoMax;

        return true;
    }

    // Falha termodinâmica ou deslocamento impossível
    _swMedia = swi;
    return false;
}

// --- Getters de Diagnóstico ---

double CWelge::getSwFrente() const {
    return _swFrente;
}

double CWelge::getSwMedia() const {
    return _swMedia;
}

double CWelge::getInclinacaoChoque() const {
    return _inclinacaoMax;
}