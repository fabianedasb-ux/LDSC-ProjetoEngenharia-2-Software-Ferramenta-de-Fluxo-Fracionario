/**
 * @file CCurvasPermeabilidadeTabelada.cpp
 * @brief Implementação das lógicas de leitura (parser) e interpolação numérica.
 * @author Fabiane da Silva Barros
 * @date Janeiro 2026
 */

#include "CCurvasPermeabilidadeTabelada.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

// --- Implementação da Interface ICurvasPermeabilidade ---

void CCurvasPermeabilidadeTabelada::carregarDados(const std::string& arquivo) {
    std::ifstream arq(arquivo);
    if (!arq.is_open()) {
        throw std::runtime_error("Exceção IO (Tabelado): Impossível resolver caminho ou abrir arquivo: " + arquivo);
    }

    std::string linha;
    bool lendoDados = false;

    // Limpeza de buffer de dados antigos antes do carregamento
    _sw.clear();
    _krw.clear();
    _kro.clear();

    // Rotina de varredura top-down e limpeza de ruídos no ASCII
    while (std::getline(arq, linha)) {
        if (linha.empty() || linha[0] == '#') continue;

        std::stringstream ss(linha);
        std::string palavraChave;
        ss >> palavraChave;

        // Máquina de estados simples: Início de bloco
        if (palavraChave == "DADOS_KR_INICIO") {
            lendoDados = true;
            continue;
        }

        // Fim de bloco de captura
        if (palavraChave == "FIM_DADOS") {
            lendoDados = false;
            break;
        }

        if (lendoDados) {
            std::stringstream ss_linha(linha);
            double sw, krw, kro;
            ss_linha >> sw >> krw >> kro;

            _sw.push_back(sw);
            _krw.push_back(krw);
            _kro.push_back(kro);
        }
    }

    // Validação Pós-Leitura de Integridade de Tamanho
    if (_sw.empty()) {
        throw std::runtime_error("Exceção Arquitetural: Arquivo malformado. Ausência dos delimitadores 'DADOS_KR_INICIO' e 'FIM_DADOS'.");
    }
}

double CCurvasPermeabilidadeTabelada::getKrw(double sw) const {
    return interpolar(sw, _sw, _krw);
}

double CCurvasPermeabilidadeTabelada::getKro(double sw) const {
    return interpolar(sw, _sw, _kro);
}

// --- Métodos Numéricos Auxiliares ---

double CCurvasPermeabilidadeTabelada::interpolar(double x_desejado, const std::vector<double>& vec_x, const std::vector<double>& vec_y) const {

    // Cenário Extremo de Contorno Esquerdo: Extrapolação inferior plana
    if (x_desejado <= vec_x.front()) {
        return vec_y.front();
    }

    // Cenário Extremo de Contorno Direito: Extrapolação superior plana
    if (x_desejado >= vec_x.back()) {
        return vec_y.back();
    }

    // Busca linear de ancoragem intervalar (Interval-matching)
    for (size_t i = 0; i < vec_x.size() - 1; ++i) {

        // Limitação lógica: x_i <= x <= x_i+1
        if (x_desejado >= vec_x[i] && x_desejado <= vec_x[i+1]) {

            double x0 = vec_x[i];
            double y0 = vec_y[i];
            double x1 = vec_x[i+1];
            double y1 = vec_y[i+1];

            // Proteção contra anomalias geométricas em matrizes densas (divisão por zero)
            if (x1 == x0) {
                return y0;
            }

            // Lei das proporções geométricas (Semelhança de triângulos na secante)
            return y0 + (x_desejado - x0) * (y1 - y0) / (x1 - x0);
        }
    }

    // Falha em cascata final de segurança (*fallback*)
    return vec_y.back();
}