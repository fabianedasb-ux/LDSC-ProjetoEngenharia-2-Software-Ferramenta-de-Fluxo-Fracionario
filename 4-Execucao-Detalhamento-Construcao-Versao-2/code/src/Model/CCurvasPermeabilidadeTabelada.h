#ifndef CCURVASPERMEABILIDADETABELADA_H
#define CCURVASPERMEABILIDADETABELADA_H

#include "ICurvasPermeabilidade.h"
#include <vector>
#include <string>

/**
 * @brief Implementação de curvas de permeabilidade baseadas em dados tabelados.
 *
 * Esta classe permite o uso de dados experimentais arbitrários (pontos discretos)
 * para definir as curvas de permeabilidade relativa. Os valores intermediários
 * solicitados pela simulação são obtidos através de interpolação linear entre
 * os pontos fornecidos.
 *
 * @author Fabiane Barros
 * @date Janeiro 2026
 */
class CCurvasPermeabilidadeTabelada : public ICurvasPermeabilidade {
private:
    /// Vetor com os valores de Saturação de Água da tabela (eixo X).
    std::vector<double> _sw;

    /// Vetor com os valores de Krw da tabela (eixo Y para água).
    std::vector<double> _krw;

    /// Vetor com os valores de Kro da tabela (eixo Y para óleo).
    std::vector<double> _kro;

    /**
     * @brief Realiza a interpolação linear entre os pontos da tabela.
     *
     * Algoritmo auxiliar que busca o intervalo onde `x_desejado` se encontra
     * e calcula o valor correspondente de Y usando a equação da reta.
     * Também trata casos de extrapolação (mantendo o valor da ponta).
     *
     * @param x_desejado Valor de saturação (Sw) para o qual se deseja o Kr.
     * @param vec_x Vetor contendo os dados de saturação (domínio).
     * @param vec_y Vetor contendo os dados de permeabilidade (imagem).
     * @return Valor interpolado de permeabilidade relativa.
     */
    double interpolar(double x_desejado, const std::vector<double>& vec_x, const std::vector<double>& vec_y) const;

public:
    /**
     * @brief Carrega os dados da tabela a partir de um arquivo de texto.
     *
     * O método lê o arquivo procurando por blocos de dados ou colunas
     * específicas (Sw, Krw, Kro) e preenche os vetores internos.
     *
     * @param arquivo Caminho completo ou relativo para o arquivo de dados (.txt).
     * @throws std::runtime_error Se o arquivo não puder ser aberto ou não contiver dados válidos.
     */
    void carregarDados(const std::string& arquivo) override;

    /**
     * @brief Obtém a permeabilidade relativa da água (Krw) para uma dada saturação.
     *
     * Chama o método privado `interpolar` usando o vetor `_krw`.
     *
     * @param sw Saturação de água atual.
     * @return Valor interpolado de Krw.
     */
    double getKrw(double sw) const override;

    /**
     * @brief Obtém a permeabilidade relativa do óleo (Kro) para uma dada saturação.
     *
     * Chama o método privado `interpolar` usando o vetor `_kro`.
     *
     * @param sw Saturação de água atual.
     * @return Valor interpolado de Kro.
     */
    double getKro(double sw) const override;
};

#endif
