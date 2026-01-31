/**
 * @file CCelula.cpp
 * @brief Implementação da classe CCelula.
 */

#include "CCelula.h"

// --- Construtor Padrão ---
CCelula::CCelula()
    : _saturacao(0.0), _posicao(0.0), _derivadaFluxo(0.0)
{
}

// --- Construtor Parametrizado ---
CCelula::CCelula(double sw, double x, double dfw)
    : _saturacao(sw), _posicao(x), _derivadaFluxo(dfw)
{
}

// --- Destrutor ---
CCelula::~CCelula() {
}

// --- Getters ---

double CCelula::getSaturacao() const {
    return _saturacao;
}

double CCelula::getPosicao() const {
    return _posicao;
}

double CCelula::getDerivadaFluxo() const {
    return _derivadaFluxo;
}

// --- Setters ---

void CCelula::setSaturacao(double sw) {
    _saturacao = sw;
}

void CCelula::setPosicao(double x) {
    _posicao = x;
}

void CCelula::setDerivadaFluxo(double dfw) {
    _derivadaFluxo = dfw;
}
