#ifndef CALCULADORAFLUXOFRACIONARIO_H
#define CALCULADORAFLUXOFRACIONARIO_H

#include "ICurvasPermeabilidade.h"
#include <map>
#include <string> // Incluído para std::string

/**
 * @class CalculadoraFluxoFracionario
 * @brief Classe principal que implementa o "domínio de negócio" (a lógica de engenharia).
 *
 * Esta classe é responsável por implementar a equação de Buckley-Leverett.
 * Ela depende de uma abstração (ICurvasPermeabilidade) e não de uma
 * implementação concreta, seguindo o Princípio da Inversão de Dependência.
 */
class CalculadoraFluxoFracionario {
private:
    /// Viscosidade do Óleo (cPoise)
    double _viscosidadeOleo;

    /// Viscosidade da Água (cPoise)
    double _viscosidadeAgua;

    /// Ponteiro para o modelo de Kr (Strategy Pattern)
    ICurvasPermeabilidade* _modeloKr;

public:
    /**
     * @brief Construtor da Calculadora.
     * Recebe as viscosidades e o modelo de Kr via Injeção de Dependência.
     * @param mu_o Viscosidade do Óleo (cPoise).
     * @param mu_w Viscosidade da Água (cPoise).
     * @param modelo Um ponteiro para um objeto que implementa ICurvasPermeabilidade.
     */
    CalculadoraFluxoFracionario(double mu_o, double mu_w, ICurvasPermeabilidade* modelo);

    /**
     * @brief Calcula um único ponto da curva de fluxo fracionário.
     * @param sw A saturação de água para a qual o Fw será calculado.
     * @return O valor do fluxo fracionário (fw).
     */
    double calcularFw(double sw) const;

    /**
     * @brief Gera a curva completa de Fw vs Sw, iterando sobre a saturação.
     * @param passo O incremento de Saturação (ex: 0.01 para 1%).
     * @return Um mapa (std::map) contendo os pares (Sw, Fw) da curva.
     */
    std::map<double, double> gerarCurvaCompleta(double passo) const;
};

#endif
