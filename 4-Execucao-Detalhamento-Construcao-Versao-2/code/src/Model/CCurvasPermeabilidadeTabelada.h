#ifndef CCURVASPERMEABILIDADETABELADA_H
#define CCURVASPERMEABILIDADETABELADA_H

/**
 * @file CCurvasPermeabilidadeTabelada.h
 * @brief Definição da classe CCurvasPermeabilidadeTabelada.
 */

#include "ICurvasPermeabilidade.h"
#include <vector>
#include <string>

/**
 * @class CCurvasPermeabilidadeTabelada
 * @brief Implementação empírica de curvas de permeabilidade baseadas em dados laboratoriais discretos.
 *
 * Esta classe concreta da interface ICurvasPermeabilidade permite a inserção
 * de dados experimentais arbitrários (pontos discretos retirados de ensaios de
 * laboratório *Special Core Analysis* - SCAL) para definir as funções
 * de permeabilidade relativa.
 *
 * A continuidade das funções para os cálculos diferenciais de fluxo fracionário
 * é garantida por meio de interpolação linear entre os nós discretos do domínio de saturação.
 *
 * @author Fabiane da Silva Barros
 * @date Janeiro 2026
 */
class CCurvasPermeabilidadeTabelada : public ICurvasPermeabilidade {
private:
    std::vector<double> _sw;  ///< Vetor de domínio: Valores da Saturação da fase aquosa ($S_w$).
    std::vector<double> _krw; ///< Vetor imagem: Valores da Permeabilidade relativa da Água ($k_{rw}$).
    std::vector<double> _kro; ///< Vetor imagem: Valores da Permeabilidade relativa do Óleo ($k_{ro}$).

    /**
     * @brief Executa o algoritmo numérico de Interpolação e Extrapolação Linear plana.
     *
     * Este método privado procura a faixa de domínio `[x_i, x_{i+1}]` onde o parâmetro
     * `x_desejado` se encontra. O valor da imagem é obtido pela equação geométrica da secante:
     * \f$ y = y_i + \frac{(x - x_i) \cdot (y_{i+1} - y_i)}{x_{i+1} - x_i} \f$
     * Valores fora dos limites tabelados são extrapolados mantendo a assíntota plana da borda (constante).
     *
     * @param x_desejado O valor interpolante (Saturação de água alvo).
     * @param vec_x A referência constante do vetor de coordenadas independentes (Domínio).
     * @param vec_y A referência constante do vetor de coordenadas dependentes (Imagem).
     * @return O valor escalar aproximado \f$ y(x) \f$.
     */
    double interpolar(double x_desejado, const std::vector<double>& vec_x, const std::vector<double>& vec_y) const;

public:
    /**
     * @brief Construtor padrão da classe CCurvasPermeabilidadeTabelada.
     */
    CCurvasPermeabilidadeTabelada() = default;

    /**
     * @brief Destrutor virtual da classe.
     */
    virtual ~CCurvasPermeabilidadeTabelada() = default;

    /**
     * @brief Executa a leitura sequencial ou paralela de dados estruturados em arquivo ASCII.
     *
     * O leitor (parser) varre o arquivo em busca das diretrizes de controle:
     * `DADOS_KR_INICIO` e `FIM_DADOS`. Os dados internos devem estar alinhados
     * matricialmente como: `[Sw] [Krw] [Kro]`.
     *
     * @param arquivo Caminho relativo ou absoluto do diretório do arquivo `.txt`.
     * @throws std::runtime_error Lança exceção bloqueante caso o arquivo seja inacessível ou se
     * a demarcação das colunas não for localizada.
     */
    void carregarDados(const std::string& arquivo) override;

    /**
     * @brief Computa a permeabilidade relativa aquosa contínua.
     * @param sw Saturação atual de água da malha ($S_w$).
     * @return A interpolação de \f$ k_{rw}(S_w) \f$.
     */
    double getKrw(double sw) const override;

    /**
     * @brief Computa a permeabilidade relativa oleica contínua.
     * @param sw Saturação atual de água da malha ($S_w$).
     * @return A interpolação de \f$ k_{ro}(S_w) \f$.
     */
    double getKro(double sw) const override;

    /** @brief Retorna a Saturação Irredutível (primeiro ponto da tabela). */
    double getSwi() const {
        return _sw.empty() ? 0.0 : _sw.front();
    }

    /** @brief Retorna a Saturação Máxima de Água (último ponto da tabela). */
    double getSwMax() const {
        return _sw.empty() ? 1.0 : _sw.back();
    }
};

#endif // CCURVASPERMEABILIDADETABELADA_H