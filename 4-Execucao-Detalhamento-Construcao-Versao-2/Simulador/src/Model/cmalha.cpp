#include "cmalha.h"

CMalha::CMalha()
{
    m_comprimentoTotal = 0.0;
    m_dx = 0.0;
}

void CMalha::CriarMalha(int numeroCelulas, double comprimentoTotal)
{
    m_comprimentoTotal = comprimentoTotal;

    // Calcula o tamanho de cada bloco (dx)
    if (numeroCelulas > 0) {
        m_dx = comprimentoTotal / numeroCelulas;
    }

    // Limpa qualquer sujeira anterior
    m_celulas.clear();

    // Loop para criar as células
    for(int i = 0; i < numeroCelulas; i++) {
        CCelula celulaNova;
        celulaNova.setId(i);
        celulaNova.setSaturacao(0.2); // Vamos chutar uma Sw inicial de 20%

        // Adiciona no vetor
        m_celulas.push_back(celulaNova);
    }
}

void CMalha::ExibirNoTerminal()
{
    std::cout << "--- ESTADO DA MALHA ---" << std::endl;
    std::cout << "Dx (Tamanho da celula): " << m_dx << " m" << std::endl;

    // Loop for moderno (C++11)
    for(const CCelula& cel : m_celulas) {
        std::cout << "Cel: " << cel.getId()
        << " | Sw: " << cel.getSaturacao() << std::endl;
    }
    std::cout << "-----------------------" << std::endl;
}

std::vector<CCelula>& CMalha::getCelulas() {
    return m_celulas;
}
