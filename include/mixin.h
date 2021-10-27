//
// Created by Igor Alentev on 25.10.2021.
//

#ifndef DE_PLOT_MIXIN_H
#define DE_PLOT_MIXIN_H

#endif //DE_PLOT_MIXIN_H

#include "implot.h"

template<typename T>
class Plottable {
public:
    virtual void plot(const char* name, T* xs, T* ys, int size) {
        ImPlot::PlotLine(name, xs, ys, size);
    }
};
