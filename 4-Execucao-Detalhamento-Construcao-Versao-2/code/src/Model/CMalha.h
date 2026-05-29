#ifndef CMALHA_H
#define CMALHA_H

/**
 * @file CMalha.h
 * @brief Definição da classe CMalha.
 */

#include <vector>
#include "CCelula.h"

/**
 * @class CMalha
 * @brief Representa o domínio discretizado do reservatório (o conjunto de pontos).
 *
 * Na abordagem analítica, a malha armazena o "perfil instantâneo" da simulação.
 * Ela contém um vetor de células, onde cada célula representa um estado (Posição, Saturação)
 * calculado para o tempo atual de injeção da água.
 *
 * @author Fabiane da Silva Barros
 * @date Janeiro 2026
 */
class CMalha {
private:
    std::vector<CCelula> _celulas; ///< Vetor dinâmico contendo os pontos do perfil espacial.
    double _tempoAtual;            ///< O tempo adimensional (em PVI) correspondente a este perfil.

public:
    /**
     * @brief Construtor padrão da classe CMalha.
     * Inicializa a malha com um vetor vazio e tempo zero.
     */
    CMalha();

    /**
     * @brief Destrutor virtual da classe CMalha.
     * Garante a liberação adequada dos recursos do vetor.
     */
    virtual ~CMalha();

    // --- Gerenciamento de Dados ---

    /**
     * @brief Remove todas as células da malha.
     * Método utilizado antes de recalcular um novo perfil espacial para um tempo diferente.
     */
    void limpar();

    /**
     * @brief Adiciona uma nova célula ao final da malha.
     * @param celula Objeto CCelula já configurado (contendo x, Sw e derivada).
     */
    void adicionarCelula(const CCelula& celula);

    /**
     * @brief Ordena as células com base na posição espacial (x) de forma crescente.
     * Fundamental para garantir a plotagem correta dos gráficos, evitando que as linhas fiquem embaralhadas.
     */
    void ordenarPorPosicao();

    // --- Setters e Getters ---

    /**
     * @brief Define o tempo atual associado ao perfil da malha.
     * @param tempo O valor do tempo adimensional (PVI).
     */
    void setTempoAtual(double tempo);

    /**
     * @brief Retorna o tempo atual do perfil armazenado na malha.
     * @return O tempo em PVI.
     */
    double getTempoAtual() const;

    /**
     * @brief Acesso direto ao vetor de células (somente leitura).
     * @return Referência constante ao vetor de células da malha.
     */
    const std::vector<CCelula>& getCelulas() const;

    /**
     * @brief Acesso direto ao vetor de células (leitura e escrita).
     * Útil se a classe CSolver precisar manipular ou iterar sobre o vetor diretamente para aplicar correções.
     * @return Referência modificável ao vetor de células.
     */
    std::vector<CCelula>& getCelulas();
};

#endif // CMALHA_H