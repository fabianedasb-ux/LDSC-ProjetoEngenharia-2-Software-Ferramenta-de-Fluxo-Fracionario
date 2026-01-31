/**
 * @file CCalculadoraFluxoFracionario.cpp
 * @brief Implementação da lógica de cálculo do fluxo fracionário.
 */

#include "CCalculadoraFluxoFracionario.h"
#include <stdexcept>
#include <cmath>
#include <algorithm> // Para std::clamp

// Define PI caso não esteja definido
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// --- Construtor ---
CCalculadoraFluxoFracionario::CCalculadoraFluxoFracionario(double mu_o, double mu_w, ICurvasPermeabilidade* modelo, double g)
    : _mi_o(mu_o), _mi_w(mu_w), _g(g), _modeloKr(modelo) // _g antes de _modeloKr
{
    // ... resto do código (inicializações seguras e validação de nullptr)
    _rho_o = 0.0;
    _rho_w = 0.0;
    _k = 0.0;
    _ut = 1.0;
    _angulo = 0.0;

    if (_modeloKr == nullptr) {
        throw std::runtime_error("Erro: Modelo de permeabilidade nulo passado para a calculadora.");
    }
}

// --- Configuração ---
void CCalculadoraFluxoFracionario::setPropriedades(double mi_w, double mi_o, double rho_w, double rho_o, double k, double angulo, double ut) {
    _mi_w = mi_w;
    _mi_o = mi_o;
    _rho_w = rho_w;
    _rho_o = rho_o;
    _k = k;
    // Converte graus para radianos para uso nas funções trigonométricas
    _angulo = angulo * (M_PI / 180.0);
    _ut = ut;
}

void CCalculadoraFluxoFracionario::setModeloPermeabilidade(ICurvasPermeabilidade* modelo) {
    if (modelo != nullptr) {
        _modeloKr = modelo;
    }
}


// --- Cálculo do Número de Rapoport-Leas (N_RL) ---
// Baseado na Seção 3.2.5, Equação 3.9 da documentação [cite: 507]
double CCalculadoraFluxoFracionario::calcularRapoportLeas(double L, double phi, double sigma) const {
    // Evita divisão por zero
    if (sigma <= 0 || _k <= 0 || phi <= 0) return 0.0;

    // NRL = (L * ut * mi_w) / (sigma * sqrt(k * phi))
    double numerador = L * _ut * _mi_w;
    double denominador = sigma * std::sqrt(_k * phi);

    return numerador / denominador;
}

// --- Cálculo Principal (Fw) ---
// Implementa a Equação 3.10 para reservatórios inclinados
double CCalculadoraFluxoFracionario::calcularFw(double sw) const {
    // 1. Obter Permeabilidades Relativas do Modelo (Strategy) [cite: 660, 723]
    double krw = _modeloKr->getKrw(sw);
    double kro = _modeloKr->getKro(sw);

    // 2. Verificação de Mobilidade para Água
    // Se não há fluxo de água, fw é zero [cite: 368]
    if (krw <= 1e-12) return 0.0;
    if (_mi_w <= 1e-9 || _mi_o <= 1e-9) return 0.0;

    // 3. Razão de Mobilidade (M) - Termo Viscoso [cite: 408, 439]
    // M = (krw / mi_w) / (kro / mi_o)
    double M = (krw / _mi_w) / (kro / _mi_o);

    // 4. Número de Gravidade (Ng) - Termo Gravitacional [cite: 440]
    // Ng = (k * kro * delta_rho * g * sin(alpha)) / (ut * mi_o)
    double Ng = 0.0;
    if (std::abs(_ut) > 1e-12) {
        double delta_rho = _rho_w - _rho_o; // [cite: 384]
        Ng = (_k * kro * delta_rho * _g * std::sin(_angulo)) / (_ut * _mi_o);
    }

    // 5. Equação de Generalizada (Eq. 3.10)
    // fw = (1 - Ng) / (1 + (1/M))
    // Note que 1/M é equivalente a (kro/krw) * (mi_w/mi_o)
    double denominador = 1.0 + (1.0 / M);
    double numerador = 1.0 - Ng;

    double fw_total = numerador / denominador;

    // Restrições Físicas: 0 <= fw <= 1
    return std::clamp(fw_total, 0.0, 1.0);
}

// --- Derivada Numérica (Diferenças Finitas) ---
double CCalculadoraFluxoFracionario::calcularDerivadaFw(double sw) const {
    double h = 1e-5; // Passo pequeno

    // Proteção de bordas
    double sw_mais = sw + h;
    double sw_menos = sw - h;
    if (sw_mais > 1.0) sw_mais = 1.0;
    if (sw_menos < 0.0) sw_menos = 0.0;

    double fw_mais = calcularFw(sw_mais);
    double fw_menos = calcularFw(sw_menos);

    if (std::abs(sw_mais - sw_menos) < 1e-9) return 0.0;

    return (fw_mais - fw_menos) / (sw_mais - sw_menos);
}

// --- Gerador de Curva ---
std::map<double, double> CCalculadoraFluxoFracionario::gerarCurvaCompleta(double passo) const {
    std::map<double, double> curva;
    if (passo <= 0) passo = 0.01;

    // Calculamos o número de divisões (inteiro) para evitar erros de precisão
    int numPontos = static_cast<int>(1.0 / passo);

    for (int i = 0; i <= numPontos; ++i) {
        double sw = i * passo;
        if (sw > 1.0) sw = 1.0; // Garante o limite físico
        curva[sw] = calcularFw(sw);
    }

    // Garante que o ponto final (1.0) esteja sempre presente
    if (curva.find(1.0) == curva.end()) {
        curva[1.0] = calcularFw(1.0);
    }

    return curva;
}
