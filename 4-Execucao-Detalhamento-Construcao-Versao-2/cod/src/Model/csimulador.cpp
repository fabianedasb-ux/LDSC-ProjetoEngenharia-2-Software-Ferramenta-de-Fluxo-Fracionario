#include "csimulador.h"

// Inicializa a calculadora com valores padrão (serão sobrepostos depois)
// Importante: Passamos nullptr no modelo inicialmente, cuidado!
CSimulador::CSimulador()
: m_calculadora(1.0, 1.0, nullptr)
{
    m_porosidade = 0.2;
    m_area = 100.0;
    m_comprimento = 1000.0;
    m_vazaoTotal = 10.0;
}

CSimulador::~CSimulador()
{
    // Se houver limpeza de memória manual necessária
}

void CSimulador::setDadosReservatorio(double porosidade, double area, double comprimento, double injecaoTotal)
{
    m_porosidade = porosidade;
    m_area = area;
    m_comprimento = comprimento;
    m_vazaoTotal = injecaoTotal;
}

void CSimulador::setModeloPermeabilidade(ICurvasPermeabilidade *modelo)
{
    // Atualiza o modelo dentro da calculadora
    // Nota: Dependendo da sua implementação da Calculadora, você pode precisar
    // criar um método 'setModelo' nela se ele só aceitar no construtor.
    // Se a Calculadora só aceita no construtor, precisaremos recriá-la aqui.

    // Assumindo que você adicionou um método setModelo na Calculadora (recomendado):
    // m_calculadora.setModelo(modelo);

    // SE NÃO TIVER setModelo:
    // Teremos que recriar o objeto. Mas como é membro stack, melhor adicionar o setter lá.
}

void CSimulador::setViscosidades(double mu_o, double mu_w)
{
    // Mesmo caso acima: ideal ter setters na CalculadoraFluxoFracionario
    // m_calculadora.setViscosidades(mu_o, mu_w);
}

void CSimulador::Executar(double tempo)
{
    // 1. Prepara a malha (cria os pontos de saturação Sw=0 a Sw=1)
    m_malha.CriarMalha(100); // 100 pontos para o gráfico ficar bonito
    m_malha.InicializarSaturacoes(); // Preenche Sw de 0 a 1

    // 2. Chama o solver para calcular o avanço (x)
    m_solver.CalcularAvanco(tempo, m_vazaoTotal, m_porosidade, m_area, m_comprimento, &m_calculadora, &m_malha);
}

CMalha *CSimulador::getMalha()
{
    return &m_malha;
}
