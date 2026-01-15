#ifndef CMALHA_H
#define CMALHA_H

#include <vector>
#include "ccelula.h"

using namespace std;

class CMalha
{
public:
    CMalha();
    ~CMalha();

    // Cria o vetor com 'n' pontos para o gráfico
    void CriarMalha(int n);

    // Método Novo: Preenche as saturações de 0 a 1 automaticamente
    // Ex: Se n=100, cria S=0.00, 0.01, 0.02...
    void InicializarSaturacoes();

    // Acesso seguro ao vetor
    int GetTamanho() const;
    CCelula& GetCelula(int i);

    // Acesso direto para plotagem (opcional, mas util para o Qt)
    const vector<CCelula>& getVetor() const;

private:
    vector<CCelula> m_malha;
};

#endif // CMALHA_H
