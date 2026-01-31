#ifndef CCURVASPERMEABILIDADECHIERICI_H
#define CCURVASPERMEABILIDADECHIERICI_H

#include "ICurvasPermeabilidade.h"
#include <cmath>
#include <string>

/**
 * @brief Implementação do modelo de permeabilidade relativa de Chierici (1984).
 *
 * A classe CCurvasPermeabilidadeChierici implementa as equações exponenciais
 * propostas por Chierici para descrever as curvas de permeabilidade relativa
 * em função da saturação normalizada. Este modelo é amplamente utilizado devido
 * à sua flexibilidade em ajustar dados experimentais.
 *
 * @author Fabiane Barros
 * @date Janeiro 2026
 */
class CCurvasPermeabilidadeChierici : public ICurvasPermeabilidade {
private:
    double _Aw;      ///< Parâmetro exponencial A para a fase aquosa.
    double _Bw;      ///< Parâmetro exponencial B para a fase aquosa.
    double _Ao;      ///< Parâmetro exponencial A para a fase oleosa.
    double _Bo;      ///< Parâmetro exponencial B para a fase oleosa.
    double _Swirr;   ///< Saturação irreduzível de água (Swir).
    double _Sor;     ///< Saturação residual de óleo (Sor).
    double _kroMax;  ///< Permeabilidade relativa máxima do óleo (no Swir).
    double _krwMax;  ///< Permeabilidade relativa máxima da água (no Sor).

public:
    /**
     * @brief Construtor principal da classe Chierici.
     *
     * Inicializa o modelo com os parâmetros empíricos e limites de saturação.
     *
     * @param Aw Parâmetro de decaimento para água.
     * @param Bw Expoente para água.
     * @param Ao Parâmetro de decaimento para óleo.
     * @param Bo Expoente para óleo.
     * @param Swirr Saturação irreduzível de água.
     * @param Sor Saturação residual de óleo.
     * @param krwMax Valor máximo de Krw (endpoint).
     * @param kroMax Valor máximo de Kro (endpoint).
     */
    CCurvasPermeabilidadeChierici(double Aw, double Bw, double Ao, double Bo,
                                  double Swirr, double Sor,
                                  double krwMax, double kroMax);

    /**
     * @brief Construtor padrão (Vazio).
     *
     * Inicializa todos os parâmetros com zero. Necessário para instanciação
     * genérica antes da configuração dos parâmetros.
     */
    CCurvasPermeabilidadeChierici();

    /**
     * @brief Destrutor virtual.
     */
    virtual ~CCurvasPermeabilidadeChierici();

    /**
     * @brief Carrega dados de um arquivo externo (Implementação da Interface).
     * * @note Nesta implementação específica, o método apenas cumpre o contrato
     * da interface, pois os parâmetros geralmente vêm da interface gráfica.
     * @param arquivo Caminho para o arquivo de dados.
     */
    void carregarDados(const std::string& arquivo) override;

    /**
     * @brief Calcula a permeabilidade relativa da água (Krw).
     *
     * Utiliza a equação: Krw = KrwMax * exp(-Aw * Swn^(-Bw))
     *
     * @param Sw Saturação de água atual.
     * @return Valor de Krw calculado (entre 0 e KrwMax).
     */
    double getKrw(double Sw) const override;

    /**
     * @brief Calcula a permeabilidade relativa do óleo (Kro).
     *
     * Utiliza a equação: Kro = KroMax * exp(-Ao * (1-Swn)^(-Bo))
     *
     * @param Sw Saturação de água atual.
     * @return Valor de Kro calculado (entre 0 e KroMax).
     */
    double getKro(double Sw) const override;
};

#endif
