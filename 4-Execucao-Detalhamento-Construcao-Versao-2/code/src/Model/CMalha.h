#ifndef CMALHA_H
#define CMALHA_H

#include <vector>
#include "CCelula.h"

/**
 * @brief Representa o domínio discretizado do reservatório (o conjunto de pontos).
 *
 * Na abordagem analítica, a malha armazena o "perfil instantâneo" da simulação.
 * Ela contém um vetor de células onde cada célula representa um par (Posição, Saturação)
 * calculado para o tempo atual (_tempoAtual).
 *
 * @author Fabiane Barros
 * @date Janeiro 2026
 */
class CMalha {
private:
    std::vector<CCelula> _celulas; ///< Vetor dinâmico contendo os pontos do perfil.
    double _tempoAtual;            ///< O tempo (em PVI ou dias) correspondente a este perfil.

public:
    /**
     * @brief Construtor padrão.
     */
    CMalha();

    /**
     * @brief Destrutor virtual.
     */
    virtual ~CMalha();

    // --- Gerenciamento de Dados ---

    /**
     * @brief Remove todas as células da malha.
     * Usado antes de recalcular um novo perfil de tempo.
     */
    void limpar();

    /**
     * @brief Adiciona uma nova célula ao final da malha.
     * @param celula Objeto CCelula já configurado (com x, Sw e derivada).
     */
    void adicionarCelula(const CCelula& celula);

    /**
     * @brief Ordena as células com base na posição (x) crescente.
     * Fundamental para garantir a plotagem correta dos gráficos (linhas contínuas).
     */
    void ordenarPorPosicao();

    // --- Setters e Getters ---

    void setTempoAtual(double tempo);
    double getTempoAtual() const;

    /**
     * @brief Acesso direto ao vetor de células (leitura).
     * @return Referência constante ao vetor.
     */
    const std::vector<CCelula>& getCelulas() const;

    /**
     * @brief Acesso direto ao vetor de células (escrita).
     * Útil se o Solver precisar manipular o vetor diretamente.
     * @return Referência modificável ao vetor.
     */
    std::vector<CCelula>& getCelulas();
};

#endif
