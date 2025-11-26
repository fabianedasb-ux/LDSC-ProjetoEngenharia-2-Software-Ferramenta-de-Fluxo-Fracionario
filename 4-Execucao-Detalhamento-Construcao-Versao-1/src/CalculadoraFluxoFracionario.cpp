#include "CalculadoraFluxoFracionario.h"
#include <cmath>     // Para std::pow
#include <stdexcept> // Para std::runtime_error
#include <limits>    // Para checagem de divisão por zero

/**
 * @brief Construtor da Calculadora.
 * @param mu_o Viscosidade do Óleo.
 * @param mu_w Viscosidade da Água.
 * @param modelo Ponteiro para o modelo de Kr (injetado).
 */
CalculadoraFluxoFracionario::CalculadoraFluxoFracionario(double mu_o, double mu_w, ICurvasPermeabilidade* modelo)
: _viscosidadeOleo(mu_o), _viscosidadeAgua(mu_w), _modeloKr(modelo) {

    // Validação de segurança
    if (modelo == nullptr) {
        throw std::runtime_error("Erro: Calculadora recebeu um modelo de permeabilidade nulo.");
    }
    if (mu_o <= 0 || mu_w <= 0) {
        throw std::runtime_error("Erro: Viscosidades devem ser positivas.");
    }
}

/**
 * @brief Calcula o valor do fluxo fracionário (fw).
 * @param sw Saturação de água.
 * @return O valor de fw.
 */
double CalculadoraFluxoFracionario::calcularFw(double sw) const {

    // 1. Pedir os valores de Kr para o modelo (Strategy Pattern)
    double krw = _modeloKr->getKrw(sw);
    double kro = _modeloKr->getKro(sw);

    // 2. Implementar a Equação de Buckley-Leverett

    // Razão de mobilidade da água: Lambda_w = krw / mu_w
    double lambda_w = krw / _viscosidadeAgua;

    // Razão de mobilidade do óleo: Lambda_o = kro / mu_o
    double lambda_o = kro / _viscosidadeOleo;

    // Mobilidade Total: Lambda_t = Lambda_w + Lambda_o
    double lambda_t = lambda_w + lambda_o;

    // Fórmula do Fluxo Fracionário: fw = Lambda_w / Lambda_t

    // Checagem de segurança para divisão por zero
    if (lambda_t < std::numeric_limits<double>::epsilon()) {
        // Se a mobilidade total é zero (ambos krw e kro são 0),
        // o fluxo fracionário também é zero.
        return 0.0;
    }

    return lambda_w / lambda_t;
}

/**
 * @brief Gera a curva completa de fw vs Sw.
 * @param passo O incremento de Saturação (ex: 0.01 para 1%).
 * @return Um mapa (map) contendo os pares (Sw, Fw).
 */
std::map<double, double> CalculadoraFluxoFracionario::gerarCurvaCompleta(double passo) const {
    std::map<double, double> curva;

    for (double sw = 0.0; sw <= 1.0; sw += passo) {
        curva[sw] = calcularFw(sw);
    }
    // Garante que o ponto final (1.0) seja sempre calculado
    curva[1.0] = calcularFw(1.0);

    return curva;
}
