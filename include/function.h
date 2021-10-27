//
// Created by Igor Alentev on 25.10.2021.
//

#ifndef DE_PLOT_FUNCTION_H
#define DE_PLOT_FUNCTION_H


#include "mixin.h"
#include <vector>
#include <string>

class Function : public Plottable<float> {
    std::function<float(float)> _f;
protected:
    std::string _name;
public:
    explicit Function(std::function<float(float)> f, std::string name = "");

    virtual void plot();
    virtual float at(float x);
    virtual std::string getName() { return this->_name; }
};


class IterativeFunction : public Function {
protected:
    std::function<float(float, float)> _diff;
public:
    IterativeFunction() : Function(nullptr) {}

    virtual float nextY(float x, float y) = 0;
    virtual void plot();
    virtual float at(float x);
};

class LocalErrorFunction : public Function {
    Function* _ideal;
    IterativeFunction* _toCompare;
public:
    LocalErrorFunction(Function* ideal, IterativeFunction* toCompare, std::string name);
    virtual float at(float x);
    virtual void plot();
};

class GlobalErrorFunction : public Function {
    Function* _ideal;
    IterativeFunction* _toCompare;
public:
    GlobalErrorFunction(Function* ideal, IterativeFunction* toCompare, std::string name);
    virtual float at(float x);
    virtual void plot();
};

class TotalErrorFunction : public Function {
    Function* _ideal;
    IterativeFunction* _toCompare;
public:
    TotalErrorFunction(Function* ideal, IterativeFunction* toCompare, std::string name);
    virtual float at(float n);
    virtual void plot();
};

class EulerMethod : public IterativeFunction {
public:
    EulerMethod(std::function<float(float, float)> diff);

    virtual float nextY(float x, float y);
};

class ImprovedEulerMethod : public EulerMethod {
public:
    ImprovedEulerMethod(std::function<float(float, float)> diff);

    virtual float nextY(float x, float y);
};

class RungeKuttaMethod : public IterativeFunction {
public:
    RungeKuttaMethod(std::function<float(float, float)> diff);

    virtual float nextY(float x, float y);
};

#endif //DE_PLOT_FUNCTION_H
