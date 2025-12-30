#ifndef CURVASPERMEABILIDADECOREY_H
#define CURVASPERMEABILIDADECOREY_H

#include "ICurvasPermeabilidade.h"
#include <string> // Incluído para std::string

/**
 * @class CurvasPermeabilidadeCorey
 * @brief Implementação concreta da interface ICurvasPermeabilidade para o modelo de Corey.
 *
 * Esta classe lê os 6 parâmetros do modelo de Corey do arquivo de entrada
 * e calcula Krw/Kro analiticamente usando as fórmulas de Corey.
 */
class CurvasPermeabilidadeCorey : public ICurvasPermeabilidade {
private:
    /// Saturação de água irreduzível (Swir)
    double _swir;

    /// Saturação de óleo residual (Sorw)
    double _sorw;

    /// Permeabilidade relativa máxima da água (no Sorw)
    double _krw_max;

    /// Permeabilidade relativa máxima do óleo (no Swir)
    double _kro_max;

    /// Expoente de Corey para a água (nw)
    double _nw;

    /// Expoente de Corey para o óleo (no)
    double _no;

public:
    /**
     * @brief Carrega os 6 parâmetros do modelo Corey do arquivo.
     * @param arquivo O caminho para o arquivo de configuração .txt.
     */
    void carregarDados(const std::string& arquivo) override;

    /**
     * @brief Calcula o Krw usando a fórmula de Corey.
     * @param sw A saturação de água.
     * @return O valor de Krw analítico.
     */
    double getKrw(double sw) const override;

    /**
     * @brief Calcula o Kro usando a fórmula de Corey.
     * @param sw A saturação de água.
     * @return O valor de Kro analítico.
     */
    double getKro(double sw) const override;
};

#endif
