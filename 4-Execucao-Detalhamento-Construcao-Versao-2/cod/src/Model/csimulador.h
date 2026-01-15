#ifndef CSIMULADOR_H
#define CSIMULADOR_H

#include "cmalha.h"
#include "csolver.h"
#include "CalculadoraFluxoFracionario.h"
#include "ICurvasPermeabilidade.h"

class CSimulador
{
public:
    CSimulador();
    ~CSimulador();

    // Configuração dos parâmetros físicos
    void setDadosReservatorio(double porosidade, double area, double comprimento, double injecaoTotal);

    // Injeção de dependência (estratégia de permeabilidade)
    void setModeloPermeabilidade(ICurvasPermeabilidade* modelo);
    void setViscosidades(double mu_o, double mu_w);

    // Executa a simulação analítica
    void Executar(double tempo);

    // Acesso aos resultados para o gráfico
    CMalha* getMalha();

private:
    CMalha m_malha;
    CSolver m_solver;
    CalculadoraFluxoFracionario m_calculadora; // Instância concreta

    // Parâmetros do reservatório
    double m_porosidade;
    double m_area;
    double m_comprimento;
    double m_vazaoTotal;
};

#endif // CSIMULADOR_H
