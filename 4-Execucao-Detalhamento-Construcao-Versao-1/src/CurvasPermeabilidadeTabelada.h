#ifndef CURVASPERMEABILIDADETABELADA_H
#define CURVASPERMEABILIDADETABELADA_H

#include "ICurvasPermeabilidade.h"
#include <vector>
#include <string> // Incluído para std::string

/**
 * @class CurvasPermeabilidadeTabelada
 * @brief Implementação concreta da interface ICurvasPermeabilidade para dados tabulados.
 *
 * Esta classe lê uma tabela de Sw, Krw e Kro de um arquivo de entrada e usa
 * interpolação linear para calcular valores intermediários.
 */
class CurvasPermeabilidadeTabelada : public ICurvasPermeabilidade {
private:
    /// Vetor com os valores de Saturação de Água da tabela.
    std::vector<double> _sw;

    /// Vetor com os valores de Krw da tabela.
    std::vector<double> _krw;

    /// Vetor com os valores de Kro da tabela.
    std::vector<double> _kro;

    /**
     * @brief Algoritmo de Interpolação Linear (e extrapolação de ponta).
     * Este é o algoritmo detalhado no Diagrama de Atividades.
     * @param x_desejado A saturação (Sw) que queremos.
     * @param vec_x O vetor de Saturações da tabela (_sw).
     * @param vec_y O vetor de Krw ou Kro correspondente.
     * @return O valor de y (Kr) interpolado.
     */
    double interpolar(double x_desejado, const std::vector<double>& vec_x, const std::vector<double>& vec_y) const;

public:
    /**
     * @brief Carrega os dados da tabela (bloco DADOS_KR_INICIO) do arquivo.
     * @param arquivo O caminho para o arquivo de configuração .txt.
     */
    void carregarDados(const std::string& arquivo) override;

    /**
     * @brief Obtém o Krw, usando interpolação se necessário.
     * @param sw A saturação de água.
     * @return O valor de Krw interpolado.
     */
    double getKrw(double sw) const override;

    /**
     * @brief Obtém o Kro, usando interpolação se necessário.
     * @param sw A saturação de água.
     * @return O valor de Kro interpolado.
     */
    double getKro(double sw) const override;
};

#endif
