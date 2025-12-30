#include "csimulador.h"
#include <iostream>

CSimulador::CSimulador()
{
    m_tempoTotal = 10.0;
    m_dt = 0.1;
    m_injecao = 1.0; // Injetando 100% água
}

void CSimulador::setDados(double tempoTotal, double dt, int numCelulas, double injecao)
{
    m_tempoTotal = tempoTotal;
    m_dt = dt;
    m_injecao = injecao;

    // Cria o reservatório físico (ex: 1000 metros)
    m_malha.CriarMalha(numCelulas, 1000.0);
}

CMalha* CSimulador::getMalha() {
    return &m_malha;
}

void CSimulador::Executar()
{
    std::cout << ">>> INICIANDO SIMULACAO (Newton-Raphson) <<<" << std::endl;

    double tempoAtual = 0.0;
    int passos = 0;

    // Loop no TEMPO
    while (tempoAtual < m_tempoTotal) {

        // Pega as células da malha para trabalhar
        // O "&" significa que vamos mexer nos dados originais, não numa cópia
        std::vector<CCelula>& celulas = m_malha.getCelulas();

        // Loop no ESPAÇO (Varre todas as células)
        // Atenção: Começamos da primeira até a última
        for (size_t i = 0; i < celulas.size(); i++) {

            // 1. Quem é o vizinho da esquerda? (De onde vem o fluxo)
            double fluxo_entrada;

            if (i == 0) {
                // Primeira célula: Recebe a injeção do poço
                fluxo_entrada = m_injecao;
            } else {
                // Outras células: Recebem o que saiu da célula anterior (i-1)
                // Calculamos o fluxo que sai da célula anterior com a saturação DELA
                double sw_vizinho = celulas[i-1].getSaturacao();
                fluxo_entrada = m_solver.CalcularFluxo(sw_vizinho);
            }

            // 2. Preparar dados para o Newton
            double sw_atual = celulas[i].getSaturacao();
            double dx = 10.0; // Valor fixo por enquanto ou m_malha.getDx()

            // 3. Resolver Newton para achar a NOVA saturação
            // Usamos a saturação atual como "chute inicial"
            double sw_nova = m_solver.CalcularPassoNewton(sw_atual, sw_atual, m_dt, dx, fluxo_entrada);

            // 4. Atualizar a célula
            celulas[i].setSaturacao(sw_nova);
        }

        // Avança o tempo
        tempoAtual += m_dt;
        passos++;

        // Log a cada 10 passos para não travar o terminal
        if (passos % 10 == 0) {
            std::cout << "Tempo: " << tempoAtual
                      << " dias | Sw na celula 0: " << celulas[0].getSaturacao()
                      << " | Sw na celula 50: " << celulas[50].getSaturacao() << std::endl;
        }
    }

    std::cout << ">>> SIMULACAO FINALIZADA <<<" << std::endl;
}

void CSimulador::setPropriedadesFisicas(double viscOleo, double viscAgua, double porosidade)
{
    // O simulador recebe da Janela e avisa o Solver (Newton-Raphson)
    // Nota: Porosidade ainda não usamos na equação de Buckley simples, mas já guardamos
    m_solver.setViscosidades(viscOleo, viscAgua);
}
