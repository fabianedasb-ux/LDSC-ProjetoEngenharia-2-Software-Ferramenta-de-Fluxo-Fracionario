/**
 * @file CCurvasPermeabilidadeChierici.cpp
 * @brief Implementação analítica das equações exponenciais do modelo Chierici.
 * @author Fabiane da Silva Barros
 * @date Janeiro 2026
 */

#include "CCurvasPermeabilidadeChierici.h"
#include <iostream>
#include <fstream>
#include <stdexcept>

// --- Construtor Principal ---
CCurvasPermeabilidadeChierici::CCurvasPermeabilidadeChierici(double Aw, double Bw, double Ao, double Bo,
                                                             double Swirr, double Sor,
                                                             double krwMax, double kroMax)
    : _Aw(Aw), _Bw(Bw), _Ao(Ao), _Bo(Bo),
    _Swirr(Swirr), _Sor(Sor), _kroMax(kroMax), _krwMax(krwMax)
{
}

// --- Construtor Vazio ---
CCurvasPermeabilidadeChierici::CCurvasPermeabilidadeChierici()
    : _Aw(0.0), _Bw(0.0), _Ao(0.0), _Bo(0.0),
    _Swirr(0.0), _Sor(0.0), _kroMax(0.0), _krwMax(0.0)
{
}

// --- Destrutor ---
CCurvasPermeabilidadeChierici::~CCurvasPermeabilidadeChierici() {
}

// --- Implementação da Interface ICurvasPermeabilidade ---

void CCurvasPermeabilidadeChierici::carregarDados(const std::string& arquivo) {
    std::ifstream in(arquivo);
    if (!in.is_open()) {
        throw std::runtime_error("Exceção Chierici: Falha de acesso ao arquivo de entrada: " + arquivo);
    }

    // Leitura estrita de 8 parâmetros formatados
    if (!(in >> _Aw >> _Bw >> _Ao >> _Bo >> _Swirr >> _Sor >> _kroMax >> _krwMax)) {
        throw std::runtime_error("Exceção Chierici: Avaria no formato dos dados. Esperado: Aw Bw Ao Bo Swirr Sor KroMax KrwMax");
    }

    in.close();
}

double CCurvasPermeabilidadeChierici::getKrw(double Sw) const {
    double denominador = 1.0 - _Swirr - _Sor;

    // Proteção de estabilidade estrutural
    if (denominador <= 1e-6) return 0.0;

    double Swn = (Sw - _Swirr) / denominador;

    // Condições de contorno (Singularidade assintótica prevenida)
    if (Swn <= 0.0) return 0.0;
    if (Swn >= 1.0) return _krwMax;

    // Matemática Exponencial: Krw = KrwMax * exp(-Aw * Swn^(-Bw))
    return _krwMax * std::exp(-_Aw * std::pow(Swn, -_Bw));
}

double CCurvasPermeabilidadeChierici::getKro(double Sw) const {
    double denominador = 1.0 - _Swirr - _Sor;

    if (denominador <= 1e-6) return 0.0;

    double Swn = (Sw - _Swirr) / denominador;

    // Condições de contorno (Singularidade assintótica prevenida)
    if (Swn >= 1.0) return 0.0;
    if (Swn <= 0.0) return _kroMax;

    // Matemática Exponencial: Kro = KroMax * exp(-Ao * (1-Swn)^(-Bo))
    return _kroMax * std::exp(-_Ao * std::pow(1.0 - Swn, -_Bo));
}