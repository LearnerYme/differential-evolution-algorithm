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