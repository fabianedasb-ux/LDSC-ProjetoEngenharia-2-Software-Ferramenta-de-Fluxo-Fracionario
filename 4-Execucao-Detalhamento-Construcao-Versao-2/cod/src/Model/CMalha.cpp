#include "CMalha.h"

CMalha::CMalha()
{
}

CMalha::~CMalha()
{
    m_malha.clear();
}

void CMalha::CriarMalha(int n)
{
    // Apenas redimensiona o vetor de pontos
    m_malha.resize(n);
}

void CMalha::InicializarSaturacoes()
{
    int n = m_malha.size();
    if (n <= 1) return;

    // Distribui as saturações de 0.0 a 1.0 ao longo dos pontos
    double passo = 1.0 / (n - 1);

    for (int i = 0; i < n; i++) {
        double sw = i * passo;
        m_malha[i].setSaturacao(sw);
        m_malha[i].setX(0.0); // Posição começa em 0
    }
}

int CMalha::GetTamanho() const
{
    return m_malha.size();
}

CCelula& CMalha::GetCelula(int i)
{
    return m_malha[i];
}

const vector<CCelula>& CMalha::getVetor() const
{
    return m_malha;
}
