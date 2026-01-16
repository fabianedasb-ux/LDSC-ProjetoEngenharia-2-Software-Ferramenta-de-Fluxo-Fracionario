#ifndef ICURVASPERMEABILIDADE_H
#define ICURVASPERMEABILIDADE_H

#include <string>

class ICurvasPermeabilidade {
public:
    virtual ~ICurvasPermeabilidade() {}

    virtual void carregarDados(const std::string& arquivo) = 0;
    virtual double getKrw(double sw) const = 0;
    virtual double getKro(double sw) const = 0;
};

#endif
