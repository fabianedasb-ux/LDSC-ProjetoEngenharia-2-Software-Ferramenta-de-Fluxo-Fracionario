#ifndef GNUPLOT_H
#define GNUPLOT_H

#include <map>
#include <string>

/**
 * @class Gnuplot
 * @brief Classe utilitária que implementa o Padrão de Projeto Facade.
 *
 * Esta classe esconde toda a complexidade de lidar com o Gnuplot
 * (criar arquivos temporários, formatar scripts, chamar o system())
 * por trás de um único método estático.
 */
class Gnuplot {
public:
    /**
     * @brief Plota um mapa de dados (x, y) usando o Gnuplot.
     * Este é um método estático, pois não precisa de um estado de instância.
     * @param dados Um std::map<double, double> onde a chave é o eixo X (Sw) e o valor é o eixo Y (Fw).
     * @param titulo O título que aparecerá no topo do gráfico.
     */
    static void plotarCurva(const std::map<double, double>& dados, const std::string& titulo);
};

#endif
