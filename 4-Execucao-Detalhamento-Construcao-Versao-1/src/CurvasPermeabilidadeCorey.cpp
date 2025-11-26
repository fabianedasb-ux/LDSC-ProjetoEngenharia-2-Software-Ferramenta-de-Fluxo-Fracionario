#include "CurvasPermeabilidadeCorey.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cmath>     // Para std::pow
#include <algorithm> // Para std::max e std::min

/**
 * @brief Carrega os parâmetros do modelo de Corey do arquivo de entrada.
 * @param arquivo O caminho para o arquivo de configuração .txt.
 */
void CurvasPermeabilidadeCorey::carregarDados(const std::string& arquivo) {
    // Seta valores padrão inválidos para checagem
    _swir = _sorw = _krw_max = _kro_max = _nw = _no = -1.0;

    std::ifstream arq(arquivo);
    if (!arq.is_open()) {
        throw std::runtime_error("Erro (Corey): Nao foi possivel abrir o arquivo: " + arquivo);
    }

    std::string linha;
    std::string palavraChave;

    while (std::getline(arq, linha)) {
        if (linha.empty() || linha[0] == '#') continue;

        std::stringstream ss(linha);
        ss >> palavraChave;

        // Lê e armazena cada parâmetro
        if (palavraChave == "COREY_SWIR")     ss >> _swir;
        else if (palavraChave == "COREY_SORW")    ss >> _sorw;
        else if (palavraChave == "COREY_KRW_MAX") ss >> _krw_max;
        else if (palavraChave == "COREY_KRO_MAX") ss >> _kro_max;
        else if (palavraChave == "COREY_NW")      ss >> _nw;
        else if (palavraChave == "COREY_NO")      ss >> _no;
    }

    // Validação simples
    if (_swir < 0 || _sorw < 0 || _krw_max < 0 || _kro_max < 0 || _nw < 0 || _no < 0) {
        throw std::runtime_error("Erro: Um ou mais parametros do modelo Corey nao foram carregados corretamente.");
    }
    std::cout << "DEBUG: Parametros de Corey carregados com sucesso.\n";
}

/**
 * @brief Calcula a Saturação Normalizada (Sw_norm).
 * Privado, apenas para uso interno desta classe.
 */
double calcularSwNorm(double sw, double swir, double sorw) {
    // Fórmula: Sw_norm = (Sw - Swir) / (1 - Swir - Sorw)
    double sw_norm = (sw - swir) / (1.0 - swir - sorw);

    // Limita o resultado entre 0 e 1 (clamp)
    return std::max(0.0, std::min(1.0, sw_norm));
}

/**
 * @brief Calcula Krw usando a fórmula de Corey.
 * @param sw Saturação de água.
 * @return Valor de Krw.
 */
double CurvasPermeabilidadeCorey::getKrw(double sw) const {
    // 1. Calcular Sw_norm
    double sw_norm = calcularSwNorm(sw, _swir, _sorw);

    // 2. Fórmula de Corey para Krw: krw = krw_max * (Sw_norm ^ nw)
    return _krw_max * std::pow(sw_norm, _nw);
}

/**
 * @brief Calcula Kro usando a fórmula de Corey.
 * @param sw Saturação de água.
 * @return Valor de Kro.
 */
double CurvasPermeabilidadeCorey::getKro(double sw) const {
    // 1. Calcular Sw_norm
    double sw_norm = calcularSwNorm(sw, _swir, _sorw);

    // 2. Fórmula de Corey para Kro: kro = kro_max * ((1 - Sw_norm) ^ no)
    return _kro_max * std::pow(1.0 - sw_norm, _no);
}
