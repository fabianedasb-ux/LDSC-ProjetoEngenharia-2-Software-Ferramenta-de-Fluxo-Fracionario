#include "CalculadoraFluxoFracionario.h"
#include <cmath> // Para abs se precisar

// ... (Mantenha seu construtor existente) ...

void CalculadoraFluxoFracionario::setViscosidades(double mu_o, double mu_w) {
    _viscosidadeOleo = mu_o;
    _viscosidadeAgua = mu_w;
}

void CalculadoraFluxoFracionario::setModeloPermeabilidade(ICurvasPermeabilidade* modelo) {
    // Aqui apenas atualizamos o ponteiro.
    // Quem chama (CSimulador/MainWindow) é responsável por gerenciar a memória do modelo antigo se necessário.
    _modeloKr = modelo;
}

// ... (Mantenha calcularFw existente) ...

// Implementação da Derivada (Central Difference)
double CalculadoraFluxoFracionario::calcularDerivadaFw(double sw) const {
    double h = 0.00001; // Passo pequeno
    double sw_mais = sw + h;
    double sw_menos = sw - h;

    // Proteção de bordas
    if (sw_mais > 1.0) sw_mais = 1.0;
    if (sw_menos < 0.0) sw_menos = 0.0;

    double fw_mais = calcularFw(sw_mais);
    double fw_menos = calcularFw(sw_menos);

    // Diferença finita central
    if (abs(sw_mais - sw_menos) < 1e-9) return 0.0; // Evita divisão por zero
    return (fw_mais - fw_menos) / (sw_mais - sw_menos);
}
