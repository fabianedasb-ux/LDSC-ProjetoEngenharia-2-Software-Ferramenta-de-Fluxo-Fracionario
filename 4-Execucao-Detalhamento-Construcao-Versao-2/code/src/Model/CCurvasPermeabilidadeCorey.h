#ifndef CCURVASPERMEABILIDADECOREY_H
#define CCURVASPERMEABILIDADECOREY_H

#include "ICurvasPermeabilidade.h"
#include <string>
#include <cmath>

/**
 * @brief Implementação do modelo de permeabilidade relativa de Corey.
 *
 * O modelo de Corey é uma correlação do tipo potência amplamente utilizada
 * na engenharia de reservatórios. Ele assume que a permeabilidade relativa
 * é proporcional a uma potência da saturação normalizada.
 *
 * Equações:
 * Krw = Krw_max * (Swn)^Nw
 * Kro = Kro_max * (1 - Swn)^No
 *
 * @author Fabiane Barros
 * @date Janeiro 2026
 */
class CCurvasPermeabilidadeCorey : public ICurvasPermeabilidade {
private:
    double _swir;    ///< Saturação irreduzível de água (Swir).
    double _sorw;    ///< Saturação residual de óleo (Sor).
    double _krw_max; ///< Permeabilidade relativa máxima da água (endpoint).
    double _kro_max; ///< Permeabilidade relativa máxima do óleo (endpoint).
    double _nw;      ///< Expoente de Corey para a água.
    double _no;      ///< Expoente de Corey para o óleo.

    /**
     * @brief Calcula a saturação normalizada de água.
     * @param sw Saturação real.
     * @return Saturação normalizada entre 0 e 1.
     */
    double calcularSwNorm(double sw) const;

public:
    /**
     * @brief Construtor principal parametrizado.
     *
     * @param kroMax Permeabilidade relativa máxima do óleo.
     * @param krwMax Permeabilidade relativa máxima da água.
     * @param no Expoente de Corey para óleo.
     * @param nw Expoente de Corey para água.
     * @param swir Saturação irreduzível de água.
     * @param sor Saturação residual de óleo.
     */
    CCurvasPermeabilidadeCorey(double kroMax, double krwMax, double no, double nw, double swir, double sor);

    /**
     * @brief Construtor padrão (Vazio).
     * Inicializa todos os parâmetros com zero.
     */
    CCurvasPermeabilidadeCorey();

    /**
     * @brief Destrutor virtual.
     */
    virtual ~CCurvasPermeabilidadeCorey();

    /**
     * @brief Carrega os parâmetros do modelo a partir de um arquivo.
     *
     * Espera um arquivo de texto contendo os 6 parâmetros na ordem:
     * Swir, Sor, KroMax, KrwMax, No, Nw.
     *
     * @param arquivo Caminho completo do arquivo.
     * @throws std::runtime_error Se não for possível abrir ou ler o arquivo.
     */
    void carregarDados(const std::string& arquivo) override;

    /**
     * @brief Calcula Krw (Permeabilidade Relativa da Água).
     * @param sw Saturação de água atual.
     * @return Valor de Krw.
     */
    double getKrw(double sw) const override;

    /**
     * @brief Calcula Kro (Permeabilidade Relativa do Óleo).
     * @param sw Saturação de água atual.
     * @return Valor de Kro.
     */
    double getKro(double sw) const override;
};

#endif
