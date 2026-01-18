#ifndef CSOLVER_H
#define CSOLVER_H

#include "CMalha.h"
#include "CCalculadoraFluxoFracionario.h"

class CSolver
{
public:
    CSolver();

    /**
     * @brief Calcula a nova posição (x) para cada saturação da malha usando Buckley-Leverett.
     * Formula: x = (Qt / (phi * A)) * fw'(Sw) * tempo
     */
    void CalcularAvanco(double tempoTotal,
                        double vazaoTotal,
                        double porosidade,
                        double areaSecao,
                        double comprimentoMax,
                        CCalculadoraFluxoFracionario* calculadora,
                        CMalha* malha);

private:
    // Método auxiliar para calcular a derivada numericamente (funciona para Corey, LET, Chierici)
    double obterDerivadaNumerica(CCalculadoraFluxoFracionario* calc, double Sw);
};

#endif // CSOLVER_H
