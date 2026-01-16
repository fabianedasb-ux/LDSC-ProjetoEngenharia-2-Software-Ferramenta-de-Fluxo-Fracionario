#ifndef CCURVASPERMEABILIDADELET_H
#define CCURVASPERMEABILIDADELET_H

#include "ICurvasPermeabilidade.h"
#include <cmath>

class CurvasPermeabilidadeLET : public ICurvasPermeabilidade {
private:
    // Parâmetros do modelo LET
    double _Lw, _Ew, _Tw;
    double _Lo, _Eo, _To;
    double _Swirr, _Sor;

public:
    // Construtor
    CurvasPermeabilidadeLET(double Lw, double Ew, double Tw,
                            double Lo, double Eo, double To,
                            double Swirr, double Sor)
        : _Lw(Lw), _Ew(Ew), _Tw(Tw), _Lo(Lo), _Eo(Eo), _To(To),
        _Swirr(Swirr), _Sor(Sor) {}

    // Implementação estrita da interface (getKrw)
    double getKrw(double Sw) override {
        double Swn = (Sw - _Swirr) / (1.0 - _Swirr - _Sor);
        if (Swn <= 0) return 0.0;
        if (Swn >= 1) return 1.0;
        // Fórmula LET Água
        return pow(Swn, _Lw) / (pow(Swn, _Lw) + _Ew * pow(1.0 - Swn, _Tw));
    }

    // Implementação estrita da interface (getKro)
    double getKro(double Sw) override {
        double Swn = (Sw - _Swirr) / (1.0 - _Swirr - _Sor);
        if (Swn <= 0) return 1.0;
        if (Swn >= 1) return 0.0;
        // Fórmula LET Óleo
        return pow(1.0 - Swn, _Lo) / (pow(1.0 - Swn, _Lo) + _Eo * pow(Swn, _To));
    }
};

#endif
