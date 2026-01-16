#include "CCelula.h"

CCelula::CCelula() : m_x(0.0), m_saturacao(0.0)
{
}

double CCelula::getX() const
{
    return m_x;
}

void CCelula::setX(double newX)
{
    m_x = newX;
}

double CCelula::getSaturacao() const
{
    return m_saturacao;
}

void CCelula::setSaturacao(double newSaturacao)
{
    m_saturacao = newSaturacao;
}
