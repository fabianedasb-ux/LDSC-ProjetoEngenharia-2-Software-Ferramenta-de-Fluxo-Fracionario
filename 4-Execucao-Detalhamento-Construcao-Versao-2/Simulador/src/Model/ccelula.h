#ifndef CCELULA_H
#define CCELULA_H

class CCelula
{
public:
    CCelula(); // Construtor

    // Métodos para definir valores (Setters)
    void setId(int id);
    void setSaturacao(double sw);
    void setPressao(double p);

    // Métodos para ler valores (Getters)
    int getId() const;
    double getSaturacao() const;
    double getPressao() const;

private:
    int m_id;             // Identificador da célula (0, 1, 2...)
    double m_saturacao;   // Saturação de Água (Sw)
    double m_pressao;     // Pressão na célula
};

#endif // CCELULA_H
