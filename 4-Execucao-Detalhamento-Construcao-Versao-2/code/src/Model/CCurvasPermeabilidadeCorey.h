#ifndef CURVASPERMEABILIDADECOREY_H
#define CURVASPERMEABILIDADECOREY_H

#include "ICurvasPermeabilidade.h"
#include <string>
#include <cmath>

class CCurvasPermeabilidadeCorey : public ICurvasPermeabilidade {
private:
    double _swir;    // Saturação irreduzível de água
    double _sorw;    // Saturação residual de óleo
    double _krw_max; // Permeabilidade relativa máxima da água
    double _kro_max; // Permeabilidade relativa máxima do óleo
    double _nw;      // Expoente água
    double _no;      // Expoente óleo

public:
    // --- NOVO CONSTRUTOR: Recebe dados da Interface Gráfica ---
    CCurvasPermeabilidadeCorey(double kroMax, double krwMax, double no, double nw, double swir, double sor);

    // Construtor vazio (caso precise carregar de arquivo depois)
    CCurvasPermeabilidadeCorey();

    // Métodos da Interface
    void carregarDados(const std::string& arquivo) override;
    double getKrw(double sw) const override;
    double getKro(double sw) const override;
};

#endif

// Melhorar os comentários e incluir descrição para doxyfile
// Finalizar carregar dados
