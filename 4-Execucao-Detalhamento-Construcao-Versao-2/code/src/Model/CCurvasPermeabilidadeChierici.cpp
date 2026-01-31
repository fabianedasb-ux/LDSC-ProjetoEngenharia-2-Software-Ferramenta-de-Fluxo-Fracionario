/**
 * @file CCurvasPermeabilidadeChierici.cpp
 * @brief Implementação dos métodos da classe CCurvasPermeabilidadeChierici.
 */

#include "CCurvasPermeabilidadeChierici.h"
#include <iostream>  // Para logs de erro, se necessário
#include <fstream>
#include <stdexcept>

// --- Construtor Principal ---
CCurvasPermeabilidadeChierici::CCurvasPermeabilidadeChierici(double Aw, double Bw, double Ao, double Bo,
                                                             double Swirr, double Sor,
                                                             double krwMax, double kroMax)
    : _Aw(Aw), _Bw(Bw), _Ao(Ao), _Bo(Bo),
    _Swirr(Swirr), _Sor(Sor), _kroMax(kroMax), _krwMax(krwMax)
{
    // Opcional: Adicionar validações de entrada aqui (ex: Swirr + Sor < 1.0)
}

// --- Construtor Vazio ---
CCurvasPermeabilidadeChierici::CCurvasPermeabilidadeChierici()
    : _Aw(0.0), _Bw(0.0), _Ao(0.0), _Bo(0.0),
    _Swirr(0.0), _Sor(0.0), _kroMax(0.0), _krwMax(0.0)
{
}

// --- Destrutor ---
CCurvasPermeabilidadeChierici::~CCurvasPermeabilidadeChierici() {
    // Não há alocação dinâmica direta para limpar
}

// --- Implementação da Interface ---

void CCurvasPermeabilidadeChierici::carregarDados(const std::string& arquivo) {
    std::ifstream in(arquivo);
    if (!in.is_open()) {
        throw std::runtime_error("Erro Chierici: Nao foi possivel abrir o arquivo: " + arquivo);
    }

    // Ordem esperada no arquivo .txt (8 valores):
    // Aw Bw Ao Bo Swirr Sor KroMax KrwMax
    if (!(in >> _Aw >> _Bw >> _Ao >> _Bo >> _Swirr >> _Sor >> _kroMax >> _krwMax)) {
        throw std::runtime_error("Erro Chierici: Formato invalido. Esperado: Aw Bw Ao Bo Swirr Sor KroMax KrwMax");
    }

    in.close();
}

double CCurvasPermeabilidadeChierici::getKrw(double Sw) const {
    double denominador = 1.0 - _Swirr - _Sor;

    // Proteção contra divisão por zero e consistência física
    if (denominador <= 1e-6) return 0.0;

    // Cálculo da Saturação Normalizada (Swn)
    double Swn = (Sw - _Swirr) / denominador;

    // Limites físicos: Se Swn <= 0, a água não flui (está na irredutível)
    if (Swn <= 0.0) return 0.0;
    // Se Swn >= 1, atingiu o máximo de água móvel
    if (Swn >= 1.0) return _krwMax;

    // Fórmula Exponencial de Chierici para Água
    // Krw = KrwMax * exp(-Aw * Swn^(-Bw))
    // Nota: pow(Swn, -Bw) é equivalente a 1 / (Swn^Bw)
    return _krwMax * std::exp(-_Aw * std::pow(Swn, -_Bw));
}

double CCurvasPermeabilidadeChierici::getKro(double Sw) const {
    double denominador = 1.0 - _Swirr - _Sor;

    if (denominador <= 1e-6) return 0.0;

    double Swn = (Sw - _Swirr) / denominador;

    // Limites físicos: Se Swn >= 1, só tem água móvel, óleo não flui
    if (Swn >= 1.0) return 0.0;
    // Se Swn <= 0, o óleo está na saturação máxima (KroMax)
    if (Swn <= 0.0) return _kroMax;

    // Fórmula Exponencial de Chierici para Óleo
    // Kro = KroMax * exp(-Ao * (1-Swn)^(-Bo))
    return _kroMax * std::exp(-_Ao * std::pow(1.0 - Swn, -_Bo));
}
