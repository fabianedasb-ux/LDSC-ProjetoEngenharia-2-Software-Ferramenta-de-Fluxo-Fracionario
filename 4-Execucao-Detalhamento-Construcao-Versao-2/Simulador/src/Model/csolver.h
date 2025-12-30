#ifndef CSOLVER_H
#define CSOLVER_H

#include <cmath> // Para pow() e abs()
#include <iostream>

class CSolver
{
public:
    CSolver();

    // --- FÍSICA (Modelo de Corey) ---
    // Calcula f_w (Fluxo Fracionário)
    double CalcularFluxo(double sw);

    // Calcula df_w/dSw (A Derivada) -> ESSENCIAL PARA NEWTON-RAPHSON
    double CalcularDerivadaFluxo(double sw);

    // --- MATEMÁTICA (Newton-Raphson) ---
    // Resolve a equação não-linear R(sw) = 0 para descobrir a nova saturação
    double CalcularPassoNewton(double sw_old, double sw_chute, double dt, double dx, double fluxo_entrada);

    // Setters para os parâmetros de Corey (depois virão da Interface)
    void setViscosidades(double mi_o, double mi_w);
    void setExpoentes(double no, double nw);

private:
    // Parâmetros físicos
    double m_mi_o; // Viscosidade oleo
    double m_mi_w; // Viscosidade agua
    double m_no;   // Expoente oleo
    double m_nw;   // Expoente agua
    double m_sor;  // Saturação residual oleo
    double m_swr;  // Saturação irredutivel agua
};

#endif // CSOLVER_H
