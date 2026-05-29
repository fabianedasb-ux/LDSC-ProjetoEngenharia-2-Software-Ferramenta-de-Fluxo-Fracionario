/**
 * @file CMalha.cpp
 * @brief Implementação dos métodos da classe CMalha.
 * @author Fabiane da Silva Barros
 * @date Janeiro 2026
 */

#include "CMalha.h"
#include <algorithm> // Necessário para a função std::sort

// --- Construtor ---
CMalha::CMalha() : _tempoAtual(0.0) {
    // Vetor de células inicia vazio automaticamente
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
    // Utiliza std::sort da STL com uma função lambda para comparar as posições (x)
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