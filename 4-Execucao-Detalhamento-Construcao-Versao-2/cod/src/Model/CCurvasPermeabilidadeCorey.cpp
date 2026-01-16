#include "CCurvasPermeabilidadeCorey.h"
#include <algorithm> // Para std::max, std::min

// --- IMPLEMENTAÇÃO DO NOVO CONSTRUTOR ---
CCurvasPermeabilidadeCorey::CCurvasPermeabilidadeCorey(double kroMax, double krwMax, double no, double nw, double swir, double sor)
    : _swir(swir), _sorw(sor), _krw_max(krwMax), _kro_max(kroMax), _nw(nw), _no(no)
{
    // O construtor já inicializa as variáveis acima com a lista de inicialização
}

// Construtor vazio
CCurvasPermeabilidadeCorey::CCurvasPermeabilidadeCorey() {
    _swir = _sorw = _krw_max = _kro_max = _nw = _no = 0.0;
}

// Função auxiliar interna (não precisa estar no .h se for apenas static ou helper)
double calcularSwNorm(double sw, double swir, double sorw) {
    double denominador = 1.0 - swir - sorw;
    if (denominador <= 0.000001) return 0.0; // Evita divisão por zero se dados forem ruins

    double sw_norm = (sw - swir) / denominador;
    return std::max(0.0, std::min(1.0, sw_norm)); // Garante entre 0 e 1
}

double CCurvasPermeabilidadeCorey::getKrw(double sw) const {
    double sw_norm = calcularSwNorm(sw, _swir, _sorw);
    return _krw_max * std::pow(sw_norm, _nw);
}

double CCurvasPermeabilidadeCorey::getKro(double sw) const {
    double sw_norm = calcularSwNorm(sw, _swir, _sorw);
    return _kro_max * std::pow(1.0 - sw_norm, _no);
}

// Mantemos o carregarDados para compatibilidade, caso queira usar arquivos no futuro
void CCurvasPermeabilidadeCorey::carregarDados(const std::string& arquivo) {
    // Implementação antiga de leitura de arquivo (pode deixar vazia se não for usar agora)
    (void)arquivo;
}
