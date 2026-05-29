#ifndef ICURVASPERMEABILIDADE_H
#define ICURVASPERMEABILIDADE_H

/**
 * @file ICurvasPermeabilidade.h
 * @brief Definição da interface ICurvasPermeabilidade.
 */

#include <string>

/**
 * @class ICurvasPermeabilidade
 * @brief Interface (Classe Abstrata) para modelos empíricos de permeabilidade relativa.
 *
 * Define o contrato rigoroso que todos os modelos de permeabilidade (Corey, LET,
 * Chierici e Tabelado) devem implementar. A arquitetura baseia-se no padrão de
 * projeto comportamental Strategy, permitindo que o simulador principal e a
 * calculadora geométrica troquem o modelo matemático em tempo de execução sem
 * necessitar de qualquer alteração estrutural na sua lógica interna.
 *
 * @author Fabiane da Silva Barros
 * @date Janeiro 2026
 */
class ICurvasPermeabilidade {
public:
    /**
     * @brief Destrutor virtual da interface.
     * Fundamental em polimorfismo C++ para garantir a chamada em cascata dos destrutores
     * e a destruição segura e completa das instâncias das classes derivadas na memória dinâmica.
     */
    virtual ~ICurvasPermeabilidade() {}

    /**
     * @brief Carrega parâmetros ou dados discretizados a partir de um arquivo de disco externo.
     * @param arquivo Caminho absoluto ou relativo para o arquivo de texto (.txt) contendo os coeficientes empíricos ou a tabela de dados.
     */
    virtual void carregarDados(const std::string& arquivo) = 0;

    /**
     * @brief Calcula a permeabilidade relativa da fase aquosa (krw) para uma dada saturação.
     * @param sw Saturação de água atual na célula (variável independente).
     * @return Valor adimensional da permeabilidade relativa da água (frequentemente no intervalo de 0 a 1).
     */
    virtual double getKrw(double sw) const = 0;

    /**
     * @brief Calcula a permeabilidade relativa da fase oleica (kro) para uma dada saturação.
     * @param sw Saturação de água atual na célula (variável independente).
     * @return Valor adimensional da permeabilidade relativa do óleo (frequentemente no intervalo de 0 a 1).
     */
    virtual double getKro(double sw) const = 0;
};

#endif // ICURVASPERMEABILIDADE_H