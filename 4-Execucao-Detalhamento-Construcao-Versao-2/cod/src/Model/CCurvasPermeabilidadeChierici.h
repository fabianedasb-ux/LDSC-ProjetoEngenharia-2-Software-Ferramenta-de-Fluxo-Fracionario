#ifndef CCURVASPERMEABILIDADECHIERICI_H
#define CCURVASPERMEABILIDADECHIERICI_H

#include "ICurvasPermeabilidade.h"
#include <cmath>

class CurvasPermeabilidadeChierici : public ICurvasPermeabilidade {
private:
    double _Aw, _Bw, _Ao, _Bo;
    double _Swirr, _Sor;
    double _kroMax, _krwMax;

public:
    CurvasPermeabilidadeChierici(double Aw, double Bw, double Ao, double Bo,
                                 double Swirr, double Sor,
                                 double krwMax, double kroMax)
        : _Aw(Aw), _Bw(Bw), _Ao(Ao), _Bo(Bo),
        _Swirr(Swirr), _Sor(Sor), _kroMax(kroMax), _krwMax(krwMax) {}

    double getKrw(double Sw) override {
        double Swn = (Sw - _Swirr) / (1.0 - _Swirr - _Sor);
        if (Swn <= 0) return 0.0;
        // Chierici base
        return _krwMax * exp(-_Aw * pow(Swn, -_Bw));
    }

    double getKro(double Sw) override {
        double Swn = (Sw - _Swirr) / (1.0 - _Swirr - _Sor);
        if (Swn >= 1) return 0.0;
        // Chierici base
        return _kroMax * exp(-_Ao * pow(1.0 - Swn, -_Bo));
    }
};

#endif
