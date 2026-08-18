#ifndef CCURVASPERMEABILIDADECOREY_H
#define CCURVASPERMEABILIDADECOREY_H

/**
 * @file CCurvasPermeabilidadeCorey.h
 * @brief Definição da classe CCurvasPermeabilidadeCorey.
 */

#include "ICurvasPermeabilidade.h"
#include <string>
#include <cmath>

/**
 * @class CCurvasPermeabilidadeCorey
 * @brief Implementação do modelo analítico de permeabilidade relativa de Corey.
 *
 * O modelo de Corey é uma correlação empírica clássica do tipo potência,
 * amplamente utilizada na engenharia de reservatórios para sistemas bifásicos.
 * Ele assume que as permeabilidades relativas são proporcionais a uma potência
 * da saturação normalizada de água ($S_{wn}$).
 *
 * As equações fundamentais são dadas por:
 * - \f$ k_{rw} = k_{rw,max} \cdot (S_{wn})^{n_w} \f$
 * - \f$ k_{ro} = k_{ro,max} \cdot (1 - S_{wn})^{n_o} \f$
 *
 * Sendo a saturação normalizada:
 * \f$ S_{wn} = \frac{S_w - S_{wir}}{1 - S_{wir} - S_{or}} \f$
 *
 * @author Fabiane da Silva Barros
 * @date Janeiro 2026
 */
class CCurvasPermeabilidadeCorey : public ICurvasPermeabilidade {
private:
    double _swir;    ///< Saturação irredutível de água ($S_{wir}$).
    double _sor;    ///< Saturação residual de óleo ($S_{or}$).
    double _krw_max; ///< Permeabilidade relativa máxima da água (endpoint).
    double _kro_max; ///< Permeabilidade relativa máxima do óleo (endpoint).
    double _nw;      ///< Expoente empírico de Corey para a fase aquosa.
    double _no;      ///< Expoente empírico de Corey para a fase oleica.

    /**
     * @brief Calcula a saturação normalizada efetiva de água.
     * @param sw Saturação real instantânea.
     * @return Saturação normalizada ($S_{wn}$), limitada estritamente entre 0.0 e 1.0.
     */
    double calcularSwNorm(double sw) const;

public:
    /**
     * @brief Construtor parametrizado completo.
     *
     * @param kroMax Permeabilidade relativa máxima do óleo.
     * @param krwMax Permeabilidade relativa máxima da água.
     * @param no Expoente da curva de óleo.
     * @param nw Expoente da curva de água.
     * @param swir Saturação irredutível de água.
     * @param sor Saturação residual de óleo.
     */
    CCurvasPermeabilidadeCorey(double kroMax, double krwMax, double no, double nw, double swir, double sor);

    /**
     * @brief Construtor padrão.
     * Inicializa instâncias temporárias com todos os coeficientes zerados.
     */
    CCurvasPermeabilidadeCorey();

    /**
     * @brief Destrutor virtual.
     */
    virtual ~CCurvasPermeabilidadeCorey();

    /**
     * @brief Carrega os coeficientes do modelo a partir de um arquivo externo de texto.
     *
     * Espera-se um arquivo plano estruturado com 6 valores numéricos separados por espaços:
     * Swir, Sor, KroMax, KrwMax, No, Nw.
     *
     * @param arquivo Caminho absoluto ou relativo do arquivo `.txt`.
     * @throws std::runtime_error Se o arquivo for inacessível ou estiver corrompido.
     */
    void carregarDados(const std::string& arquivo) override;

    /**
     * @brief Calcula a Permeabilidade Relativa da fase Água ($k_{rw}$).
     * @param sw Saturação espacial de água atual.
     * @return Valor adimensional interpolado.
     */
    double getKrw(double sw) const override;

    /**
     * @brief Calcula a Permeabilidade Relativa da fase Óleo ($k_{ro}$).
     * @param sw Saturação espacial de água atual.
     * @return Valor adimensional interpolado.
     */
    double getKro(double sw) const override;
    /** @brief Retorna a Saturação Irredutível. */
    double getSwi() const { return _swir; } // Verifique se o atributo interno chama-se _swir ou _swirr na sua classe

    /** @brief Retorna a Saturação de óleo residual. */
    double getSor() const { return _sor; }

    // Bueno: documentar; acessadas em void MainWindow::sincronizarDadosComSimulador()
    double getKro_max() const { return _kro_max; };
    double getKrw_max() const { return _krw_max; };
    double getnw() const { return _nw; };
    double getno() const { return _no; };
};


#endif // CCURVASPERMEABILIDADECOREY_H