#include "CCalculadoraFluxoFracionario.h"
#include "ICurvasPermeabilidade.h"
#include <cmath>
#include <stdexcept>
#include <map>

// ---------------------------------------------------------
// 1. CONSTRUTOR (Fundamental para criar a classe)
// ---------------------------------------------------------
CCalculadoraFluxoFracionario::CCalculadoraFluxoFracionario(double mu_o, double mu_w, ICurvasPermeabilidade* modelo)
    : _viscosidadeOleo(mu_o), _viscosidadeAgua(mu_w), _modeloKr(modelo)
{
    // Validações básicas
    if (_modeloKr == nullptr) {
        throw std::runtime_error("Erro: Modelo de permeabilidade nulo passado para a calculadora.");
    }
}

// ---------------------------------------------------------
// 2. SETTERS (Para mudar valores depois)
// ---------------------------------------------------------
void CCalculadoraFluxoFracionario::setViscosidades(double mu_o, double mu_w) {
    _viscosidadeOleo = mu_o;
    _viscosidadeAgua = mu_w;
}

void CCalculadoraFluxoFracionario::setModeloPermeabilidade(ICurvasPermeabilidade* modelo) {
    if (modelo != nullptr) {
        _modeloKr = modelo;
    }
}

// ---------------------------------------------------------
// 3. CALCULAR FW (A lógica principal de Buckley-Leverett)
// ---------------------------------------------------------
double CCalculadoraFluxoFracionario::calcularFw(double sw) const {
    // Busca as permeabilidades no modelo (Corey, Tabelado, etc)
    double krw = _modeloKr->getKrw(sw);
    double kro = _modeloKr->getKro(sw);

    // Evita divisão por zero nas viscosidades
    if (_viscosidadeAgua <= 0 || _viscosidadeOleo <= 0) return 0.0;

    // Calcula as mobilidades (Lambda = k / mi)
    double lambda_w = krw / _viscosidadeAgua;
    double lambda_o = kro / _viscosidadeOleo;
    double lambda_t = lambda_w + lambda_o;

    // Se mobilidade total for zero, não há fluxo
    if (lambda_t < 1e-9) return 0.0;

    // Fluxo fracionário = mobilidade da água / mobilidade total
    return lambda_w / lambda_t;
}

// ---------------------------------------------------------
// 4. DERIVADA NUMÉRICA (Necessária para o CSolver avançar o tempo)
// ---------------------------------------------------------
double CCalculadoraFluxoFracionario::calcularDerivadaFw(double sw) const {
    double h = 0.00001; // Passo pequeno
    double sw_mais = sw + h;
    double sw_menos = sw - h;

    // Proteção de bordas (não sair de 0 a 1)
    if (sw_mais > 1.0) sw_mais = 1.0;
    if (sw_menos < 0.0) sw_menos = 0.0;

    // Chama a função calcularFw que implementamos acima
    double fw_mais = calcularFw(sw_mais);
    double fw_menos = calcularFw(sw_menos);

    // Diferença finita central
    if (std::abs(sw_mais - sw_menos) < 1e-9) return 0.0;
    return (fw_mais - fw_menos) / (sw_mais - sw_menos);
}

// ---------------------------------------------------------
// 5. GERAR CURVA COMPLETA (Para o Gráfico)
// ---------------------------------------------------------
std::map<double, double> CCalculadoraFluxoFracionario::gerarCurvaCompleta(double passo) const {
    std::map<double, double> curva;

    if (passo <= 0) passo = 0.01; // Proteção contra loop infinito

    for (double sw = 0.0; sw <= 1.0; sw += passo) {
        curva[sw] = calcularFw(sw);
    }
    // Garante o ponto final exato em 1.0
    curva[1.0] = calcularFw(1.0);

    return curva;
}
