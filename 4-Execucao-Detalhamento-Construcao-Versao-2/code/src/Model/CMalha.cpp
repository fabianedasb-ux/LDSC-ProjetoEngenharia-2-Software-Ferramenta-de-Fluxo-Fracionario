/**
 * @file CMalha.cpp
 * @brief Implementação da classe CMalha.
 */

#include "CMalha.h"
#include <algorithm> // Para std::sort

// --- Construtor ---
CMalha::CMalha() : _tempoAtual(0.0) {
    // Vetor inicia vazio
}

// --- Destrutor ---
CMalha::~CMalha() {
    limpar();
}

// --- Gerenciamento ---

void CMalha::limpar() {
    _celulas.clear();
}

void CMalha::adicionarCelula(const CCelula& celula) {
    _celulas.push_back(celula);
}

void CMalha::ordenarPorPosicao() {
    // Utiliza std::sort com uma função lambda para comparar as posições
    std::sort(_celulas.begin(), _celulas.end(),
              [](const CCelula& a, const CCelula& b) {
                  return a.getPosicao() < b.getPosicao();
              }
              );
}

// --- Setters e Getters ---

void CMalha::setTempoAtual(double tempo) {
    _tempoAtual = tempo;
}

double CMalha::getTempoAtual() const {
    return _tempoAtual;
}

const std::vector<CCelula>& CMalha::getCelulas() const {
    return _celulas;
}

std::vector<CCelula>& CMalha::getCelulas() {
    return _celulas;
}
