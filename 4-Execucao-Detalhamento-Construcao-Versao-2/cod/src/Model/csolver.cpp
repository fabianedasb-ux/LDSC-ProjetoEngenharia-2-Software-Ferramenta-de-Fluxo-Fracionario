#include "csolver.h"
#include <algorithm> // Para std::min

CSolver::CSolver()
{
}

void CSolver::CalcularAvanco(double tempoTotal, double vazaoTotal, double porosidade, double areaSecao, double comprimentoMax, CalculadoraFluxoFracionario *calculadora, CMalha *malha)
{
    // Constante de velocidade: v_t = qt / (phi * A)
    // Se as unidades não estiverem consistentes, adicione conversão aqui.
    // Assumindo tudo no SI ou Lab units consistentes.
    double velocidadeBase = vazaoTotal / (porosidade * areaSecao);

    int n = malha->GetTamanho();

    // Loop para calcular a posição x de cada saturação
    for(int i = 0; i < n; i++)
    {
        CCelula& celula = malha->GetCelula(i);
        double Sw = celula.getSaturacao();

        // 1. Calcula a derivada dfw/dSw
        double derivada = obterDerivadaNumerica(calculadora, Sw);

        // 2. Calcula a posição: x = v * t * f'(Sw)
        double novaPosicao = velocidadeBase * tempoTotal * derivada;

        // Limita ao comprimento do reservatório
        if (novaPosicao > comprimentoMax) novaPosicao = comprimentoMax;
        if (novaPosicao < 0) novaPosicao = 0;

        celula.setX(novaPosicao);
    }

    // TODO: Aqui entrará a lógica de corte do CHOQUE (Welge) na próxima etapa.
    // Por enquanto, ele vai plotar a curva "multivalorada" (em S), o que é normal no início.
}

double CSolver::obterDerivadaNumerica(CalculadoraFluxoFracionario *calc, double Sw)
{
    double h = 0.0001; // Pequeno passo para derivada
    double sw_mais = Sw + h;
    double sw_menos = Sw - h;

    // Garante limites [0,1]
    if (sw_mais > 1.0) sw_mais = 1.0;
    if (sw_menos < 0.0) sw_menos = 0.0;

    double fw_mais = calc->calcularFw(sw_mais);
    double fw_menos = calc->calcularFw(sw_menos);

    return (fw_mais - fw_menos) / (sw_mais - sw_menos);
}
