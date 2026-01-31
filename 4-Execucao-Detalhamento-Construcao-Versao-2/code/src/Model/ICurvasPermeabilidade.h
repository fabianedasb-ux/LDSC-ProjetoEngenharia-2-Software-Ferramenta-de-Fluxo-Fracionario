#ifndef ICURVASPERMEABILIDADE_H
#define ICURVASPERMEABILIDADE_H

#include <string>

/**
 * @brief Interface (Classe Abstrata) para modelos de permeabilidade relativa.
 *
 * Define o contrato que todos os modelos de permeabilidade (Corey, LET,
 * Chierici, Tabelado) devem implementar. Utiliza o padrão de projeto Strategy,
 * permitindo que o simulador troque o modelo matemático em tempo de execução
 * sem alterar sua lógica interna.
 *
 * @author Fabiane Barros
 * @date Janeiro 2026
 */
class ICurvasPermeabilidade {
public:
    /**
     * @brief Destrutor virtual.
     * Necessário para garantir a destruição correta das classes derivadas.
     */
    virtual ~ICurvasPermeabilidade() {}

    /**
     * @brief Carrega parâmetros ou dados de um arquivo externo.
     * @param arquivo Caminho completo para o arquivo de configuração (.txt).
     */
    virtual void carregarDados(const std::string& arquivo) = 0;

    /**
     * @brief Calcula a permeabilidade relativa da água (Krw).
     * @param sw Saturação de água atual.
     * @return Valor adimensional de Krw (0 a 1).
     */
    virtual double getKrw(double sw) const = 0;

    /**
     * @brief Calcula a permeabilidade relativa do óleo (Kro).
     * @param sw Saturação de água atual.
     * @return Valor adimensional de Kro (0 a 1).
     */
    virtual double getKro(double sw) const = 0;
};

#endif
