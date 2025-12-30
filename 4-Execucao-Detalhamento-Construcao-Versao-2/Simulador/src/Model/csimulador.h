#ifndef CSIMULADOR_H
#define CSIMULADOR_H

#include "cmalha.h"
#include "csolver.h"

class CSimulador
{
public:
    CSimulador();

    // Configura a simulação
    void setDados(double tempoTotal, double dt, int numCelulas, double injecao);

    // Roda a simulação completa (do tempo 0 até o fim)
    void Executar();

    // Pega a malha para podermos ver os resultados depois
    CMalha* getMalha();

    // Novo método para passar dados físicos
    void setPropriedadesFisicas(double viscOleo, double viscAgua, double porosidade);

private:
    CMalha m_malha;
    CSolver m_solver;

    double m_tempoTotal;
    double m_dt;
    double m_injecao; // Fluxo de água injetado na primeira célula (f_w = 1.0 geralmente)
};

#endif // CSIMULADOR_H
