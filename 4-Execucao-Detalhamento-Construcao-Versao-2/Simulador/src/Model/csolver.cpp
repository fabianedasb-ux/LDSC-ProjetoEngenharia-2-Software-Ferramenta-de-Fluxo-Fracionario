#include "csolver.h"

CSolver::CSolver()
{
    // Valores padrão (depois a interface vai mudar isso)
    m_mi_o = 2.0;   // cP
    m_mi_w = 1.0;   // cP
    m_no = 2.0;     // Corey Oil
    m_nw = 2.0;     // Corey Water
    m_sor = 0.2;
    m_swr = 0.2;
}

void CSolver::setViscosidades(double mi_o, double mi_w) {
    m_mi_o = mi_o;
    m_mi_w = mi_w;
}

void CSolver::setExpoentes(double no, double nw) {
    m_no = no;
    m_nw = nw;
}

// 1. O Modelo de Fluxo (A Física)
double CSolver::CalcularFluxo(double sw)
{
    // Limites físicos (Saturação Normalizada)
    if (sw <= m_swr) return 0.0;
    if (sw >= (1.0 - m_sor)) return 1.0;

    double Swn = (sw - m_swr) / (1.0 - m_swr - m_sor); // Normalização

    // Permeabilidades Relativas (Corey)
    double kro = pow(1.0 - Swn, m_no);
    double krw = pow(Swn, m_nw);

    // Fluxo Fracionário (Lei de Darcy simplificada para fw)
    // fw = 1 / (1 + (kro * mi_w) / (krw * mi_o))
    double mobilidade_ratio = (kro * m_mi_w) / (krw * m_mi_o);
    return 1.0 / (1.0 + mobilidade_ratio);
}

// 2. A Derivada (A Ferramenta do Newton-Raphson)
// Aqui usamos Diferença Finita simples para não ter que derivar na mão agora
double CSolver::CalcularDerivadaFluxo(double sw)
{
    double epsilon = 1e-5;
    double fw_mais = CalcularFluxo(sw + epsilon);
    double fw_menos = CalcularFluxo(sw - epsilon);

    return (fw_mais - fw_menos) / (2.0 * epsilon);
}

// 3. O Algoritmo de Newton-Raphson (O Motor)
double CSolver::CalcularPassoNewton(double sw_old, double sw_chute, double dt, double dx, double fluxo_vizinho)
{
    // Equação Discretizada Implícita (Simplificada para 1 célula):
    // R(S_new) = S_new - S_old + (dt/dx) * (fw(S_new) - fw_vizinho) = 0

    double S_k = sw_chute; // Começamos com o chute (geralmente S_k = S_old)
    int max_iter = 20;
    double tolerancia = 1e-6;

    std::cout << "   -> Iniciando Newton para Sw_old=" << sw_old << std::endl;

    for(int k = 0; k < max_iter; k++) {

        // Passo A: Calcular o Fluxo e a Derivada no ponto atual k
        double fw_k = CalcularFluxo(S_k);
        double dfw_k = CalcularDerivadaFluxo(S_k);

        // Passo B: Montar a Função Resíduo R(S_k)
        // R = Acúmulo + Fluxo Liquido
        double R = (S_k - sw_old) + (dt / dx) * (fw_k - fluxo_vizinho);

        // Passo C: Montar a Derivada do Resíduo R'(S_k)
        // R' = 1 + (dt/dx) * fw'
        double R_linha = 1.0 + (dt / dx) * dfw_k;

        // Passo D: Atualizar S (Fórmula de Newton-Raphson)
        double delta_S = -R / R_linha;
        double S_novo = S_k + delta_S;

        // Debug simples
        // std::cout << "      Iter " << k << ": Sw=" << S_k << " Res=" << R << std::endl;

        // Critério de Parada
        if (std::abs(delta_S) < tolerancia) {
            return S_novo; // Convergiu!
        }

        S_k = S_novo; // Prepara para próxima iteração
    }

    std::cout << "AVISO: Newton nao convergiu!" << std::endl;
    return S_k; // Retorna o melhor que conseguiu
}
