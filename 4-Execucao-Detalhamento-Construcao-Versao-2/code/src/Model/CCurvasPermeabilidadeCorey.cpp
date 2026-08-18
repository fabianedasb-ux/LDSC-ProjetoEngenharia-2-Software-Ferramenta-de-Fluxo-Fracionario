/**
 * @file CCurvasPermeabilidadeCorey.cpp
 * @brief Implementação dos métodos do modelo de permeabilidade relativa de Corey.
 * @author Fabiane da Silva Barros
 * @date Janeiro 2026
 */

#include "CCurvasPermeabilidadeCorey.h"
#include <algorithm> // Para os limitadores std::max, std::min
#include <fstream>   // Para leitura de streams de disco (std::ifstream)
#include <stdexcept> // Para lançamento de exceções em tempo de execução

// --- Construtor Parametrizado ---
CCurvasPermeabilidadeCorey::CCurvasPermeabilidadeCorey(double kroMax, double krwMax, double no, double nw, double swir, double sor)
    : _swir(swir), _sor(sor), _krw_max(krwMax), _kro_max(kroMax), _nw(nw), _no(no)
{
}

// --- Construtor Padrão ---
CCurvasPermeabilidadeCorey::CCurvasPermeabilidadeCorey()
    : _swir(0.0), _sor(0.0), _krw_max(0.0), _kro_max(0.0), _nw(0.0), _no(0.0)
{
}

// --- Destrutor ---
CCurvasPermeabilidadeCorey::~CCurvasPermeabilidadeCorey() {
}

// --- Método Auxiliar Privado ---
double CCurvasPermeabilidadeCorey::calcularSwNorm(double sw) const {
    double denominador = 1.0 - _swir - _sor;

    // Tratamento numérico para evitar singularidade e divisão por zero
    // corrigir: numero magico, 1e-6 deve estar na interface
    if (denominador <= 1e-6) return 0.0;

    double sw_norm = (sw - _swir) / denominador;

    // Assegura robustez física: Swn nunca será negativo ou maior que a unidade
    return std::max(0.0, std::min(1.0, sw_norm));
}

// --- Implementação da Interface ICurvasPermeabilidade ---

void CCurvasPermeabilidadeCorey::carregarDados(const std::string& arquivo) {
    std::ifstream in(arquivo);
    if (!in.is_open()) {
        throw std::runtime_error("Exceção Corey: Falha na montagem do arquivo -> " + arquivo);
    }

    // Processo de extração serial dos 6 coeficientes estruturais
    if (!(in >> _swir >> _sor >> _kro_max >> _krw_max >> _no >> _nw)) {
        throw std::runtime_error("Exceção Corey: Erro no formato numérico de entrada.");
    }

    in.close();
}

double CCurvasPermeabilidadeCorey::getKrw(double sw) const {
    double sw_norm = calcularSwNorm(sw);
    // Eq: krw = krw_max * (Swn)^nw
    return _krw_max * std::pow(sw_norm, _nw);
}

double CCurvasPermeabilidadeCorey::getKro(double sw) const {
    double sw_norm = calcularSwNorm(sw);
    // Eq: kro = kro_max * (1 - Swn)^no
    return _kro_max * std::pow(1.0 - sw_norm, _no);
}