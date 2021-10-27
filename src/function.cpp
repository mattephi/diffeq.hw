//
// Created by Igor Alentev on 25.10.2021.
//

#include <utility>
#include <SystemConfiguration.h>
#include "function.h"
#include "implot.h"

Function::Function(std::function<float(float)> f, std::string name) : _f(std::move(f)), _name(std::move(name)) {}

void Function::plot() {
    float step = (EquationConfiguration::get().x_max - EquationConfiguration::get().x_0) / (float)EquationConfiguration::get().n;
    float xs[EquationConfiguration::get().n + 1], ys[EquationConfiguration::get().n + 1];
    for (int i = 0; i < EquationConfiguration::get().n + 1; ++i) {
        xs[i] = EquationConfiguration::get().x_0 + step * (float)i;
        ys[i] = this->at(xs[i]);
    }
    Plottable::plot(_name.c_str(), xs, ys, EquationConfiguration::get().n + 1);
}

float Function::at(float x) {
    return _f(x);
}

void IterativeFunction::plot() {
    float step = (EquationConfiguration::get().x_max - EquationConfiguration::get().x_0) / (float)EquationConfiguration::get().n;
    float xs[EquationConfiguration::get().n + 1], ys[EquationConfiguration::get().n + 1];
    xs[0] = EquationConfiguration::get().x_0;
    ys[0] = EquationConfiguration::get().y_0;
    for (int i = 1; i < EquationConfiguration::get().n + 1; ++i) {
        xs[i] = xs[i - 1] + step;
        ys[i] = nextY(xs[i - 1], ys[i - 1]);
    }
    Plottable::plot(_name.c_str(), xs, ys, EquationConfiguration::get().n + 1);
}

float IterativeFunction::at(float x) {
    const float eps = 1e-2;
    if (fabs(x - EquationConfiguration::get().x_0) < eps) {
        return EquationConfiguration::get().y_0;
    }
    float step = (EquationConfiguration::get().x_max - EquationConfiguration::get().x_0) / (float)EquationConfiguration::get().n;
    float xs[EquationConfiguration::get().n + 1], ys[EquationConfiguration::get().n + 1];
    xs[0] = EquationConfiguration::get().x_0;
    ys[0] = EquationConfiguration::get().y_0;
    for (int i = 1; i < EquationConfiguration::get().n + 1; ++i) {
        xs[i] = xs[i - 1] + step;
        ys[i] = nextY(xs[i - 1], ys[i - 1]);
        if (fabs(x - xs[i]) < eps) {
            return ys[i];
        }
    }
    return -FLT_MAX;
}

float EulerMethod::nextY(float x, float y) {
    return y + (EquationConfiguration::get().x_max - EquationConfiguration::get().x_0) / (float)EquationConfiguration::get().n * _diff(x, y);
}

EulerMethod::EulerMethod(std::function<float(float, float)> diff) {
    _diff = std::move(diff);
    _name = "Euler Method";
}

float RungeKuttaMethod::nextY(float x, float y) {
    float step = (EquationConfiguration::get().x_max - EquationConfiguration::get().x_0) / (float)EquationConfiguration::get().n;
    float k1 = _diff(x, y);
    float k2 = _diff(x + step / 2, y + k1 * step / 2);
    float k3 = _diff(x + step / 2, y + k2 * step / 2);
    float k4 = _diff(x + step, y + k3 * step);
    return y + step * (k1 + 2 * k2 + 2 * k3 + k4) / 6;
}

RungeKuttaMethod::RungeKuttaMethod(std::function<float(float, float)> diff) {
    _diff = std::move(diff);
    _name = "Runge-Kutta Method";
}

float ImprovedEulerMethod::nextY(float x, float y) {
    float step = (EquationConfiguration::get().x_max - EquationConfiguration::get().x_0) / (float)EquationConfiguration::get().n;
    return y + step / 2 * (_diff(x, y) + _diff(x + step, EulerMethod::nextY(x, y)));
}

ImprovedEulerMethod::ImprovedEulerMethod(std::function<float(float, float)> diff) : EulerMethod(std::move(diff)) {
    _name = "Improved Euler Method";
}

LocalErrorFunction::LocalErrorFunction(Function *ideal, IterativeFunction *toCompare, std::string name) : _ideal(ideal), _toCompare(toCompare), Function(nullptr, std::move(name)) {}

float LocalErrorFunction::at(float x) {
    return fabs(_toCompare->nextY(x, _ideal->at(x)) - _toCompare->at(x));
}

void LocalErrorFunction::plot() {
    Function::plot();
}

GlobalErrorFunction::GlobalErrorFunction(Function *ideal, IterativeFunction *toCompare, std::string name) : _ideal(ideal), _toCompare(toCompare), Function(nullptr, std::move(name)) {}

float GlobalErrorFunction::at(float x) {
    return fabs(_ideal->at(x) - _toCompare->at(x));
}

void GlobalErrorFunction::plot() {
    Function::plot();
}

TotalErrorFunction::TotalErrorFunction(Function *ideal, IterativeFunction *toCompare, std::string name) : _ideal(ideal), _toCompare(toCompare), Function(nullptr, std::move(name)) {}

float TotalErrorFunction::at(float n) {
    int backup = EquationConfiguration::get().n;
    EquationConfiguration::get().n = (int)std::lround(n);
    auto lte = LocalErrorFunction(_ideal, _toCompare, "");
    float step = (EquationConfiguration::get().x_max - EquationConfiguration::get().x_0) / n;
    float mx = -FLT_MAX;
    for (int i = 0; (float)i < n + 1; ++i) {
        mx = std::max(mx, lte.at(EquationConfiguration::get().x_0 + (float)i * step));
    }
    EquationConfiguration::get().n = backup;
    return mx;
}

void TotalErrorFunction::plot() {
    int min_steps = EquationConfiguration::get().n_0;
    int max_steps = EquationConfiguration::get().n;
    float xs[max_steps - min_steps + 1], ys[max_steps - min_steps + 1];
    for (int i = min_steps; i < max_steps + 1; ++i) {
        xs[i - min_steps] = (float)i;
        ys[i - min_steps] = this->at((float)i);
    }
    Plottable::plot(_name.c_str(), xs, ys, max_steps - min_steps + 1);
}
