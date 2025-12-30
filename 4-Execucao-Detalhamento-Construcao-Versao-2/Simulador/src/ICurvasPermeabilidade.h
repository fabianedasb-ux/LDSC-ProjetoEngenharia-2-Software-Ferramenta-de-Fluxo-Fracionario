#ifndef ICURVASPERMEABILIDADE_H
#define ICURVASPERMEABILIDADE_H

#include <string>

/**
 * @class ICurvasPermeabilidade
 * @brief Interface (classe base abstrata) para os modelos de permeabilidade relativa.
 *
 * Esta classe define o "contrato" que todos os modelos de Kr (Tabelado, Corey, etc.)
 * devem seguir. Ela é a base do Padrão de Projeto Strategy, permitindo que a
 * Calculadora opere em um alto nível de abstração (Inversão de Dependência).
 */
class ICurvasPermeabilidade {
public:
    /**
     * @brief Destrutor virtual padrão, essencial para classes base polimórficas.
     */
    virtual ~ICurvasPermeabilidade() {}

    /**
     * @brief Método virtual puro para carregar dados de um arquivo.
     *
     * Cada classe filha deve implementar este método para ler seus
     * parâmetros específicos do arquivo de entrada (ex: ler a tabela ou ler os parâmetros de Corey).
     * @param arquivo O caminho (path) para o arquivo de configuração .txt.
     */
    virtual void carregarDados(const std::string& arquivo) = 0;

    /**
     * @brief Obtém a permeabilidade relativa da água (Krw).
     * @param sw A saturação de água (Sw) para a qual o Krw será calculado.
     * @return O valor de Krw (entre 0 e 1).
     */
    virtual double getKrw(double sw) const = 0;

    /**
     * @brief Obtém a permeabilidade relativa do óleo (Kro).
     * @param sw A saturação de água (Sw) para a qual o Kro será calculado.
     * @return O valor de Kro (entre 0 e 1).
     */
    virtual double getKro(double sw) const = 0;
};

#endif
