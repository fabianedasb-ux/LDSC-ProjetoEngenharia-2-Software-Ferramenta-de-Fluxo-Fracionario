/**
 * @file CCurvasPermeabilidadeCorey.cpp
 * @brief Implementação da classe CCurvasPermeabilidadeCorey.
 */

#include "CCurvasPermeabilidadeCorey.h"
#include <algorithm> // Para std::max, std::min
#include <fstream>   // Para leitura de arquivos
#include <stdexcept> // Para exceções

// --- Construtor Parametrizado ---
CCurvasPermeabilidadeCorey::CCurvasPermeabilidadeCorey(double kroMax, double krwMax, double no, double nw, double swir, double sor)
    : _swir(swir), _sorw(sor), _krw_max(krwMax), _kro_max(kroMax), _nw(nw), _no(no)
{
}

// --- Construtor Vazio ---
CCurvasPermeabilidadeCorey::CCurvasPermeabilidadeCorey()
    : _swir(0), _sorw(0), _krw_max(0), _kro_max(0), _nw(0), _no(0)
{
}

// --- Destrutor ---
CCurvasPermeabilidadeCorey::~CCurvasPermeabilidadeCorey() {
}

// --- Método Auxiliar Privado ---
double CCurvasPermeabilidadeCorey::calcularSwNorm(double sw) const {
    double denominador = 1.0 - _swir - _sorw;
    if (denominador <= 1e-6) return 0.0; // Evita divisão por zero

    double sw_norm = (sw - _swir) / denominador;
    // Garante que o resultado fique entre 0.0 e 1.0 (limites físicos)
    return std::max(0.0, std::min(1.0, sw_norm));
}

// --- Implementação da Interface ---

void CCurvasPermeabilidadeCorey::carregarDados(const std::string& arquivo) {
    std::ifstream in(arquivo);
    if (!in.is_open()) {
        throw std::runtime_error("CCurvasCorey: Nao foi possivel abrir o arquivo: " + arquivo);
    }

    // Tenta ler os 6 parâmetros na ordem esperada
    // Ordem sugerida: Swir Sor KroMax KrwMax No Nw
    if (!(in >> _swir >> _sorw >> _kro_max >> _krw_max >> _no >> _nw)) {
        throw std::runtime_error("CCurvasCorey: Erro ao ler parametros numericos do arquivo.");
    }

    in.close();
}

double CCurvasPermeabilidadeCorey::getKrw(double sw) const {
    double sw_norm = calcularSwNorm(sw);
    return _krw_max * std::pow(sw_norm, _nw);
}

double CCurvasPermeabilidadeCorey::getKro(double sw) const {
    double sw_norm = calcularSwNorm(sw);
    // Para óleo, a base é (1 - Swn)
    return _kro_max * std::pow(1.0 - sw_norm, _no);
}
