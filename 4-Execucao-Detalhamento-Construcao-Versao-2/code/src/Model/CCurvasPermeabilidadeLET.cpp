/**
 * @file CCurvasPermeabilidadeLET.cpp
 * @brief Implementação dos métodos da classe CCurvasPermeabilidadeLET.
 */

#include "CCurvasPermeabilidadeLET.h"
#include <iostream>
#include <fstream>
#include <stdexcept>

// --- Construtor Principal ---
CCurvasPermeabilidadeLET::CCurvasPermeabilidadeLET(double Lw, double Ew, double Tw,
                                                   double Lo, double Eo, double To,
                                                   double Swirr, double Sor)
    : _Lw(Lw), _Ew(Ew), _Tw(Tw),
    _Lo(Lo), _Eo(Eo), _To(To),
    _Swirr(Swirr), _Sor(Sor)
{
}

// --- Construtor Vazio ---
CCurvasPermeabilidadeLET::CCurvasPermeabilidadeLET()
    : _Lw(0), _Ew(0), _Tw(0),
    _Lo(0), _Eo(0), _To(0),
    _Swirr(0), _Sor(0)
{
}

// --- Destrutor ---
CCurvasPermeabilidadeLET::~CCurvasPermeabilidadeLET() {
}

// --- Métodos da Interface ---

void CCurvasPermeabilidadeLET::carregarDados(const std::string& arquivo) {
    std::ifstream in(arquivo);
    if (!in.is_open()) {
        throw std::runtime_error("Erro LET: Nao foi possivel abrir o arquivo: " + arquivo);
    }

    // Ordem esperada no arquivo .txt (8 valores):
    // Lw Ew Tw Lo Eo To Swirr Sor
    if (!(in >> _Lw >> _Ew >> _Tw >> _Lo >> _Eo >> _To >> _Swirr >> _Sor)) {
        throw std::runtime_error("Erro LET: Formato de arquivo invalido. Esperado: Lw Ew Tw Lo Eo To Swirr Sor");
    }

    in.close();
}

double CCurvasPermeabilidadeLET::getKrw(double Sw) const {
    double denominador = 1.0 - _Swirr - _Sor;
    if (denominador <= 1e-6) return 0.0;

    // Saturação normalizada
    double Swn = (Sw - _Swirr) / denominador;

    // Limites físicos
    if (Swn <= 0.0) return 0.0;
    if (Swn >= 1.0) return 1.0; // Assume-se endpoint=1 se não especificado diferente no LET puro

    // Termos da equação LET para água
    // Numerador: Swn^L
    double num = std::pow(Swn, _Lw);

    // Denominador: Swn^L + E * (1-Swn)^T
    double term2 = _Ew * std::pow(1.0 - Swn, _Tw);

    return num / (num + term2);
}

double CCurvasPermeabilidadeLET::getKro(double Sw) const {
    double denominador = 1.0 - _Swirr - _Sor;
    if (denominador <= 1e-6) return 0.0;

    // Saturação normalizada
    double Swn = (Sw - _Swirr) / denominador;

    // Limites físicos
    if (Swn <= 0.0) return 1.0; // Assume-se endpoint=1
    if (Swn >= 1.0) return 0.0;

    // Termos da equação LET para óleo
    // Numerador: (1-Swn)^L
    double num = std::pow(1.0 - Swn, _Lo);

    // Denominador: (1-Swn)^L + E * Swn^T
    double term2 = _Eo * std::pow(Swn, _To);

    return num / (num + term2);
}
