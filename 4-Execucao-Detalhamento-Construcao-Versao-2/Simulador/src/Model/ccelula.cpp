#include "ccelula.h"

CCelula::CCelula()
{
    // Inicializa com valores padrão para não dar erro de lixo de memória
    m_id = -1;
    m_saturacao = 0.0; // Começa seco (ou com saturação irredutível depois)
    m_pressao = 0.0;
}

void CCelula::setId(int id) {
    m_id = id;
}

void CCelula::setSaturacao(double sw) {
    m_saturacao = sw;
}

void CCelula::setPressao(double p) {
    m_pressao = p;
}

int CCelula::getId() const {
    return m_id;
}

double CCelula::getSaturacao() const {
    return m_saturacao;
}

double CCelula::getPressao() const {
    return m_pressao;
}
