/**
 * @file CCurvasPermeabilidadeLET.cpp
 * @brief Implementação dos métodos e equações do modelo CCurvasPermeabilidadeLET.
 * @author Fabiane da Silva Barros
 * @date Janeiro 2026
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
    : _Lw(0.0), _Ew(0.0), _Tw(0.0),
    _Lo(0.0), _Eo(0.0), _To(0.0),
    _Swirr(0.0), _Sor(0.0)
{
}

// --- Destrutor ---
CCurvasPermeabilidadeLET::~CCurvasPermeabilidadeLET() {
}

// --- Implementação da Interface ICurvasPermeabilidade ---

void CCurvasPermeabilidadeLET::carregarDados(const std::string& arquivo) {
    std::ifstream in(arquivo);
    if (!in.is_open()) {
        throw std::runtime_error("Exceção LET: Não foi possível localizar ou abrir o arquivo: " + arquivo);
    }

    // Leitura estrita dos 8 parâmetros do modelo LET
    if (!(in >> _Lw >> _Ew >> _Tw >> _Lo >> _Eo >> _To >> _Swirr >> _Sor)) {
        throw std::runtime_error("Exceção LET: Formato numérico corrompido. Esperado: Lw Ew Tw Lo Eo To Swirr Sor");
    }

    in.close();
}

double CCurvasPermeabilidadeLET::getKrw(double Sw) const {
    double denominador = 1.0 - _Swirr - _Sor;
    if (denominador <= 1e-6) return 0.0;

    // Normalização da saturação
    double Swn = (Sw - _Swirr) / denominador;

    // Condições físicas de contorno
    if (Swn <= 0.0) return 0.0;
    if (Swn >= 1.0) return 1.0;

    // Motor matemático LET para a água
    double num = std::pow(Swn, _Lw);
    double term2 = _Ew * std::pow(1.0 - Swn, _Tw);

    return num / (num + term2);
}

double CCurvasPermeabilidadeLET::getKro(double Sw) const {
    double denominador = 1.0 - _Swirr - _Sor;
    if (denominador <= 1e-6) return 0.0;

    // Normalização da saturação
    double Swn = (Sw - _Swirr) / denominador;

    // Condições físicas de contorno
    if (Swn <= 0.0) return 1.0;
    if (Swn >= 1.0) return 0.0;

    // Motor matemático LET para o óleo
    double num = std::pow(1.0 - Swn, _Lo);
    double term2 = _Eo * std::pow(Swn, _To);

    return num / (num + term2);
}