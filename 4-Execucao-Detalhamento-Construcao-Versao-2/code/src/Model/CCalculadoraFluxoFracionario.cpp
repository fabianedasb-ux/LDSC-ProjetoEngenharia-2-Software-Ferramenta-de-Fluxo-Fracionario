/**
 * @file CCalculadoraFluxoFracionario.cpp
 * @brief Implementação da lógica termodinâmica e cálculos adimensionais do escoamento.
 * @author Fabiane da Silva Barros
 * @date Janeiro 2026
 */

#include "CCalculadoraFluxoFracionario.h"
#include <stdexcept>
#include <cmath>
#include <algorithm> // Para algoritmos de saturação

// Injeção da constante Pi (padrão C++)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// --- Construtor Padrão ---
CCalculadoraFluxoFracionario::CCalculadoraFluxoFracionario(double mu_o, double mu_w, ICurvasPermeabilidade* modelo, double g)
    : _mi_o(mu_o), _mi_w(mu_w), _g(g), _modeloKr(modelo)
{
    // Valores de inicialização segura (*fail-safe*)
    _rho_o = 800.0;
    _rho_w = 1000.0;
    _k = 100.0;
    _angulo = 0.0;
    _qt = 1.0;
    _A = 1.0;
    _ut = 1.0;

    if (_modeloKr == nullptr) {
        throw std::runtime_error("Exceção Core: A Calculadora Física exige uma estratégia de permeabilidade não nula na inicialização.");
    }
}

// --- Configuração ---
void CCalculadoraFluxoFracionario::setPropriedades(double mi_w, double mi_o, double rho_w, double rho_o, double k, double angulo, double qt, double A) {
    _mi_w = mi_w;
    _mi_o = mi_o;
    _rho_w = rho_w;
    _rho_o = rho_o;
    _k = k;

    // Transformação analítica: de Graus para Radianos
    _angulo = angulo * (M_PI / 180.0);
    _qt = qt;
    _A = A;

    // Consequência cinemática imediata:
    _ut = (qt / _A);
}

void CCalculadoraFluxoFracionario::setModeloPermeabilidade(ICurvasPermeabilidade* modelo) {
    if (modelo != nullptr) {
        _modeloKr = modelo;
    }
}

// --- Cálculo do Critério de Rapoport-Leas ---
double CCalculadoraFluxoFracionario::calcularRapoportLeas(double L, double phi, double sigma) const {
    if (sigma <= 0 || _k <= 0 || phi <= 0 || _ut <= 0) return 0.0;

    // Obtém endpoint de água avaliando em Sw = 1.0
    double krw0 = _modeloKr->getKrw(1.0);

    double numerador = L * _ut * _mi_w;
    double raiz = std::sqrt(phi / _k);

    // Assume-se reservatório wet-water forte (cos(0) = 1.0)
    double denominador = sigma * krw0 * 1.0;

    return raiz * (numerador / denominador);
}

// --- Razão de Mobilidade (M^0) ---
double CCalculadoraFluxoFracionario::calcularM0() const {
    double krw0 = _modeloKr->getKrw(1.0);
    double kro0 = _modeloKr->getKro(0.0);

    if (kro0 <= 0 || _mi_w <= 0) return 0.0;

    return (krw0 / _mi_w) / (kro0 / _mi_o);
}

// --- Número de Gravidade (Ng^0) ---
double CCalculadoraFluxoFracionario::calcularNg0() const {
    if (std::abs(_ut) < 1e-12 || _mi_o <= 0) return 0.0;

    double kro0 = _modeloKr->getKro(0.0);
    double delta_rho = _rho_w - _rho_o;

    return (_k * kro0 * delta_rho * _g) / (_ut * _mi_o);
}

// --- Núcleo Analítico: Fluxo Fracionário (fw) ---
double CCalculadoraFluxoFracionario::calcularFw(double sw) const {

    // Clamping termodinâmico para as assíntotas do deslocamento
    if (sw <= 0.0) return 0.0;
    if (sw >= 1.0) return 1.0;

    double krw = _modeloKr->getKrw(sw);
    double kro = _modeloKr->getKro(sw);

    // Condição crítica de estrangulamento de fase (Choke condition)
    if (krw < 1e-15) return 0.0;
    if (kro < 1e-15) return 1.0;

    double termo_viscoso = (kro * _mi_w) / (krw * _mi_o);
    double delta_rho = _rho_w - _rho_o;

    // Termo dependente da angulação topográfica
    double termo_gravitacional = (_k * kro * delta_rho * _g * std::sin(_angulo)) / (_ut * _mi_o);

    // O retorno pode assumir fw > 1 (contra-fluxo de óleo decantando) em falhas geológicas íngremes
    return (1.0 - termo_gravitacional) / (1.0 + termo_viscoso);
}

// --- Diferenciação Numérica (fw') ---
double CCalculadoraFluxoFracionario::calcularDerivadaFw(double sw) {
    // Abordagem de Diferenças Finitas Centrais (h = 10^-5 para mitigar erro de truncamento)
    double h = 1e-5;
    double fw_mais = calcularFw(sw + h);
    double fw_menos = calcularFw(sw - h);
    return (fw_mais - fw_menos) / (2.0 * h);
}

// --- Varredura Discreta ---
std::map<double, double> CCalculadoraFluxoFracionario::gerarCurvaCompleta(double passo) const {
    std::map<double, double> curva;
    if (passo <= 0) passo = 0.01;

    int numPontos = static_cast<int>(1.0 / passo);

    // Preenchimento contínuo do dicionário indexado
    for (int i = 0; i <= numPontos; ++i) {
        double sw = i * passo;
        if (sw > 1.0) sw = 1.0;
        curva[sw] = calcularFw(sw);
    }

    // Força o contorno de saturação absoluta
    if (curva.find(1.0) == curva.end()) {
        curva[1.0] = calcularFw(1.0);
    }

    return curva;
}