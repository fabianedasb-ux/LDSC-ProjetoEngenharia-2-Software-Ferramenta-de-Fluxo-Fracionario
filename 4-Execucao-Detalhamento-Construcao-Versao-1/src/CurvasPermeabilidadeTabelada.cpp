#include "CurvasPermeabilidadeTabelada.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

/**
 * @brief Carrega os dados tabelados (Sw, Krw, Kro) do arquivo de entrada.
 * @param arquivo O caminho para o arquivo de configuração .txt.
 */
void CurvasPermeabilidadeTabelada::carregarDados(const std::string& arquivo) {
    std::ifstream arq(arquivo);
    if (!arq.is_open()) {
        throw std::runtime_error("Erro (Tabelado): Nao foi possivel abrir o arquivo: " + arquivo);
    }

    std::string linha;
    bool lendoDados = false;

    while (std::getline(arq, linha)) {
        if (linha.empty() || linha[0] == '#') continue;

        std::stringstream ss(linha);
        std::string palavraChave;
        ss >> palavraChave; // Lê a primeira "palavra"

        if (palavraChave == "DADOS_KR_INICIO") {
            lendoDados = true;
            continue;
        }

        if (palavraChave == "FIM_DADOS") {
            lendoDados = false;
            break;
        }

        if (lendoDados) {
            // Se estamos lendo, a "palavraChave" é na verdade o valor do Sw
            std::stringstream ss_linha(linha);
            double sw, krw, kro;
            ss_linha >> sw >> krw >> kro;

            // Adiciona os valores aos vetores da classe
            _sw.push_back(sw);
            _krw.push_back(krw);
            _kro.push_back(kro);
        }
    }

    if (_sw.empty()) {
        throw std::runtime_error("Erro: Nenhum dado de permeabilidade encontrado (bloco DADOS_KR_INICIO...FIM_DADOS) no arquivo.");
    }
    std::cout << "DEBUG: " << _sw.size() << " pontos de Kr tabelados foram carregados.\n";
}

/**
 * @brief Calcula Krw para uma Sw, usando interpolação linear se necessário.
 * @param sw Saturação de água.
 * @return Valor de Krw interpolado.
 */
double CurvasPermeabilidadeTabelada::getKrw(double sw) const {
    return interpolar(sw, _sw, _krw); // Chama a função de interpolação
}

/**
 * @brief Calcula Kro para uma Sw, usando interpolação linear se necessário.
 * @param sw Saturação de água.
 * @return Valor de Kro interpolado.
 */
double CurvasPermeabilidadeTabelada::getKro(double sw) const {
    return interpolar(sw, _sw, _kro); // Chama a função de interpolação
}

/**
 * @brief Algoritmo de Interpolação Linear (e extrapolação de ponta).
 * Este é o algoritmo detalhado no Diagrama de Atividades.
 * @param x_desejado A saturação (Sw) que queremos.
 * @param vec_x O vetor de Saturações da tabela.
 * @param vec_y O vetor de Krw ou Kro correspondente.
 * @return O valor de y (Kr) interpolado.
 */
double CurvasPermeabilidadeTabelada::interpolar(double x_desejado, const std::vector<double>& vec_x, const std::vector<double>& vec_y) const {

    // Caso 1: Extrapolação (abaixo do limite inferior)
    if (x_desejado <= vec_x.front()) {
        return vec_y.front();
    }

    // Caso 2: Extrapolação (acima do limite superior)
    if (x_desejado >= vec_x.back()) {
        return vec_y.back();
    }

    // Caso 3: Interpolação (procurar o intervalo)
    for (size_t i = 0; i < vec_x.size() - 1; ++i) {

        // Achamos o intervalo: vec_x[i] <= x_desejado <= vec_x[i+1]
        if (x_desejado >= vec_x[i] && x_desejado <= vec_x[i+1]) {

            // Pontos do intervalo
            double x0 = vec_x[i];
            double y0 = vec_y[i];
            double x1 = vec_x[i+1];
            double y1 = vec_y[i+1];

            // Evita divisão por zero se os pontos Sw forem idênticos
            if (x1 == x0) {
                return y0;
            }

            // Fórmula da Interpolação Linear:
            // y = y0 + (x - x0) * (y1 - y0) / (x1 - x0)
            return y0 + (x_desejado - x0) * (y1 - y0) / (x1 - x0);
        }
    }

    // Se algo der muito errado (não deveria acontecer)
    return vec_y.back();
}
