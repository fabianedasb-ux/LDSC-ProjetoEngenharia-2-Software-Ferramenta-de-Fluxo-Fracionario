#ifndef CCELULA_H
#define CCELULA_H

class CCelula
{
public:
    CCelula();

    // Getters e Setters simplificados
    double getX() const;
    void setX(double newX);

    double getSaturacao() const;
    void setSaturacao(double newSaturacao);

private:
    double m_x;          // Posição da frente (calculada)
    double m_saturacao;  // Valor da saturação (fixo no método das características)
};

#endif // CCELULA_H
