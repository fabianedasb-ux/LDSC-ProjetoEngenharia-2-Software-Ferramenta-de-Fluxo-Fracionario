#ifndef CCALCULADORAFLUXOFRACIONARIO_H
#define CCALCULADORAFLUXOFRACIONARIO_H

#include "ICurvasPermeabilidade.h"
#include <map>

class CCalculadoraFluxoFracionario {
private:
    double _viscosidadeOleo;
    double _viscosidadeAgua;
    ICurvasPermeabilidade* _modeloKr; // Ponteiro para a estratégia (Strategy)

public:
    // Construtor
    CCalculadoraFluxoFracionario(double mu_o, double mu_w, ICurvasPermeabilidade* modelo);

    // Permite atualizar as viscosidades sem recriar o objeto
    void setViscosidades(double mu_o, double mu_w);

    // Permite trocar de Corey para LET/Chierici dinamicamente
    void setModeloPermeabilidade(ICurvasPermeabilidade* modelo);

    // calcularFw
    double calcularFw(double sw) const;

    // VERIFICAR: Necessário para a Solução Analítica (Cálculo de x)
    // O Solver precisa saber a derivada para mover a frente: dx/dt ~ dfw/dsw
    double calcularDerivadaFw(double sw) const;

    // gerarCurvaCompleta
    std::map<double, double> gerarCurvaCompleta(double passo) const;
};

#endif

// Melhorar os comentários e incluir descrição para doxyfile
