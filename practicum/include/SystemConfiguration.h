//
// Created by Igor Alentev on 26.10.2021.
//

#ifndef DE_PLOT_SYSTEMCONFIGURATION_H
#define DE_PLOT_SYSTEMCONFIGURATION_H

#include <cmath>
#include <functional>


class EquationConfiguration {
    struct Info {
        float x_0 = 2;
        float y_0 = 1;
        float x_max = 5;
        int n_0 = 1;
        int n = 2;
        float getConst() const {
            return (float)(2 - x_0 * y_0) * std::cbrt(x_0) / (x_0 * y_0 - 1);
        }
        std::function<float(float, float)> getFunction() {
            return [](float x, float y) { return - y * y / 3 - 2 / (3 * x * x); };
        }
        std::function<float(float)> getExactSolution() {
            return [](float x) {
                if (EquationConfiguration::get().x_0 * EquationConfiguration::get().y_0 == 1) {
                    return (float) 1 / x;
                }
                float c = EquationConfiguration::get().getConst();
                return (float)-(((2 * c / (c + std::cbrt(x)) - 1) - 3) / (2 * x));
            };
        }
    };
    static Info* info;
public:
    static Info& get() {
        if (info == nullptr) {
            info = new Info();
        }
        return *info;
    }
};


#endif //DE_PLOT_SYSTEMCONFIGURATION_H
