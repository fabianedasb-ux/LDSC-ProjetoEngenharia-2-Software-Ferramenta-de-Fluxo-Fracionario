#ifndef CCURVASPERMEABILIDADETABELADA_H
#define CCURVASPERMEABILIDADETABELADA_H

#include "ICurvasPermeabilidade.h"
#include <vector>
#include <string> // Incluído para std::string


class CCurvasPermeabilidadeTabelada : public ICurvasPermeabilidade {
private:
    /// Vetor com os valores de Saturação de Água da tabela.
    std::vector<double> _sw;

    /// Vetor com os valores de Krw da tabela.
    std::vector<double> _krw;

    /// Vetor com os valores de Kro da tabela.
    std::vector<double> _kro;


    double interpolar(double x_desejado, const std::vector<double>& vec_x, const std::vector<double>& vec_y) const;

public:

    void carregarDados(const std::string& arquivo) override;


    double getKrw(double sw) const override;


    double getKro(double sw) const override;
};

#endif
