#ifndef CMALHA_H
#define CMALHA_H

#include <vector>
#include <iostream> // Para usar cout
#include "ccelula.h" // A Malha precisa conhecer a Célula

class CMalha
{
public:
    CMalha();

    // Cria a malha com N células e comprimento total L
    void CriarMalha(int numeroCelulas, double comprimentoTotal);

    // Exibe o estado atual no terminal (Debug)
    void ExibirNoTerminal();

    // Acesso às células (para o simulador usar depois)
    std::vector<CCelula>& getCelulas();

private:
    std::vector<CCelula> m_celulas; // Vetor dinâmico de células
    double m_comprimentoTotal;
    double m_dx; // Tamanho de cada célula
};

#endif // CMALHA_H
