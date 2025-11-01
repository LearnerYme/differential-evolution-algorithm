# Differential Evolution Algorithm

A C++ implementation for general use DE algorithm with a simple demo.

NOTE: CERN-ROOT is needed.

Author: yghuang

Date: 01.11.2025

Version: 1.0

## Quick start with my demo

1. `make` to compile;

2. `./demo` to run;

3. `python3 make_anim.py` to make animation;

4. Check `DemoFigs` and `anim1.gif`.

## Optimize your function

To optimize a function, just implement the simple interface `BaseTarget` like the demo did.

In `FunctionTest.h`:

```c++
#ifndef __FUNCTIONTEST_HEADER__
#define __FUNCTIONTEST_HEADER__

#include "DECore.h"
#include "TF1.h"

namespace mdea {
    class MyFunction : public mdea::BaseTarget {
        public:
            MyFunction(){}
            ~MyFunction() override{}
            double FitnessFunctionRaw(const std::vector<double> pars) const { // this demo is to find the maximal of the function
                double res = 0;
                for (size_t i=0; i<pars.size(); i++){
                    double tmp = pars[i];
                    res += tmp * TMath::Sin(tmp) * TMath::Cos(2*tmp) - 2 * tmp * TMath::Sin(3*tmp);
                }
                return res;
            }
            double FitnessFunction(const std::vector<double> pars) const override { // this one is for DE optimization (minimization)
                return -FitnessFunctionRaw(pars);
            }
            static double WrapperFunction(double* x, double* p) { // for TF1
                std::vector<double> pars = {x[0]};
                MyFunction func;
                return func.FitnessFunctionRaw(pars);
            }
            int GetNFeatures() const override { return 1; }
            std::vector<Feature> GetFeatures() const override {
                std::vector<mdea::Feature> features = {mdea::Feature(0.0, 20.0, true)};
                return features;
            }
    };
}

static mdea::MyFunction func;
#endif
```

In `demo.cxx`, maybe your `main.cxx`, if you don't need to plot:

```c++
#include "DECore.h"
#include "FunctionTest.h"

// #include "TCanvas.h"
// #include "TF1.h"
// #include "TGraph.h"
// #include "TString.h"
// #include "TH1D.h"
// #include "TSystem.h"

int main()
{
    mdea::DiffEvolutionCore dea(func, 20, 0.5, 0.5, 1.3);
    // dea.mCallback = MakeAgentsPlots;
    dea.Main(100, true);
    return 0;
}
```

Note that, the DEA core is implemented for minimalization, and in the demo I was trying to find a maximal, that's why there is a `FitnessFunctionRaw` and `FitnessFunction` (to optimize the negative value using DEA), and `WrapperFunction` is for the call back function to make plots using `TF1`.

Should you need to early stop the process, you can implement an `EarlyStop` callback function.

## Disclaimer

This project is a practice for my C++ skill and I took references from [this repository](https://github.com/milsto/differential-evolution/tree/master).

To understand the DE algorithm, I read following references:

1. [Wikipedia](https://en.wikipedia.org/wiki/Differential_evolution)

2. [Zhihu](https://zhuanlan.zhihu.com/p/462522469)

3. [Raw Paper](https://cse.engineering.nyu.edu/~mleung/CS909/s04/Storn95-012.pdf)

4. [scipy](https://docs.scipy.org/doc/scipy/reference/generated/scipy.optimize.differential_evolution.html)

5. [ChatGPT](https://chatgpt.com)

During this, I also learnt a lot about modern C++ from ChatGPT and copilot, many thanks to AI.
