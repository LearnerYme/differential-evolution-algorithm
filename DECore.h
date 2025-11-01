#ifndef __DECORE_HEADER__
#define __DECORE_HEADER__

#include <iostream>
#include <vector>
#include "TMath.h"
#include "TRandom3.h"

namespace mdea {
    class Feature {
        public:
            Feature(double lower, double upper, bool constrain=false) :
            mLower(lower), mUpper(upper), mConstrain(constrain){}
            ~Feature(){}
            bool check(double& val) {
                /*
                Return true when the value IS VALID
                */
                return !(mConstrain && (val < mLower || val > mUpper));
            }
            double mLower;
            double mUpper;
            bool mConstrain;
    };

    class BaseTarget {
        public:
            BaseTarget(){}
            virtual ~BaseTarget() = default;
            virtual double FitnessFunction(const std::vector<double> pars) const = 0;
            virtual int GetNFeatures() const = 0;
            virtual std::vector<Feature> GetFeatures() const = 0; 
    };

    class DiffEvolutionCore {
        private:
            // hyper parameters
            int mNP; // Number of population members, aka. popsize 
            int mD;  // Number of parameters / dimensions / features
            double mF; // Differential weight, aka. mutation
            double mCRLow;
            double mCRHigh; // Crossover probability, when mCRLow == mCRHigh, it is fixed, otherwise it is a random number between them for each single generation -> according to scipy implementation

            // target 
            int mGeneration;
            int mMaxGeneration;
            const BaseTarget& mTarget;
            std::vector<std::vector<double>> mPopulation;
            std::vector<double> mBestFitnessPerAgent;
            int mBestIndex; // agent index
            double mBestFitness; // global best fitness

            // random number generator
            TRandom3 mRandGen;

        public:
            DiffEvolutionCore(const BaseTarget& target, int NP=20, double F=0.8, double CRLow=0.95, double CRHigh = 0.95) :
            mTarget(target), mNP(NP), mF(F), mCRLow(CRLow), mCRHigh(CRHigh), mBestIndex(-1), mBestFitness(TMath::Infinity()), mGeneration(0), mMaxGeneration(0) {
                mD = mTarget.GetNFeatures();
                mRandGen.SetSeed(4396); // fixed seed for reproducibility
            }
            ~DiffEvolutionCore(){}

            void InitializePopulation() {
                mPopulation.resize(mNP);
                mBestFitnessPerAgent.resize(mNP);
                for (auto & agent : mPopulation) {
                    agent.resize(mD);
                    for (int i=0; i<mD; i++) {
                        auto feature = mTarget.GetFeatures().at(i);
                        agent.at(i) = mRandGen.Uniform(feature.mLower, feature.mUpper);
                        mBestFitnessPerAgent.at(i) = mTarget.FitnessFunction(agent);
                        if (mBestFitnessPerAgent.at(i) < mBestFitness) {
                            mBestFitness = mBestFitnessPerAgent.at(i);
                            mBestIndex = i;
                        }
                    }
                }
            }

            void Evolve() {
                mGeneration += 1;
                double mCR; // CR for this generation
                if (mCRLow == mCRHigh) { mCR = mCRLow; }
                else { mCR = mRandGen.Uniform(mCRLow, mCRHigh); }
                for (int i=0; i<mNP; i++) { // agent loop
                    // select 3 other agents
                    int a = i, b = i, c = i;
                    while (a == i) { a = mRandGen.Integer(mNP); } 
                    while (b == i || b == a) { b = mRandGen.Integer(mNP); }
                    while (c == i || c == a || c == b) { c = mRandGen.Integer(mNP); }
                    // cross over
                    int jrand = mRandGen.Integer(mD); // unchanged feature
                    std::vector<double> mu(mD, 0.0);
                    for (int d=0; d<mD; d++) { // feature loop 
                        double nu = mPopulation.at(a).at(d) + mF * (mPopulation.at(b).at(d) - mPopulation.at(c).at(d));
                        double cCR = mRandGen.Uniform(0,1);
                        if ( cCR < mCR || d == jrand) { mu.at(d) = nu; } 
                        else { mu.at(d) = mPopulation.at(i).at(d); }
                    }
                    // check constraints
                    if (!IsValid(mu)) { // agent validation check
                        i -= 1; // redo this agent if failed
                        continue;
                    }
                    // check fitness of the very agent
                    double currentFitness = mBestFitnessPerAgent.at(i);
                    double newFitness = mTarget.FitnessFunction(mu);
                    if (newFitness < currentFitness) {
                        mPopulation.at(i) = mu;
                        mBestFitnessPerAgent.at(i) = newFitness;
                        if (newFitness < mBestFitness) {
                            mBestFitness = newFitness;
                            mBestIndex = i;
                        }
                    }
                }
            }

            void Main(int MaxGen, bool verbose=false) {
                InitializePopulation();
                mMaxGeneration = MaxGen;
                if (verbose) {
                    std::cout << "# Differential Evolution Algorithm" << std::endl;
                    std::cout << "#> Maximum Generation: " << MaxGen << std::endl;
                    std::cout << "#> Population Size: " << mNP << std::endl;
                    std::cout << "#> Mutation Factor: " << mF << std::endl;
                    if (mCRLow == mCRHigh) {
                        std::cout << "#> Crossover Probability: " << mCRLow << std::endl;
                    } else {
                        std::cout << "#> Crossover Probability Range: [" << mCRLow << ", " << mCRHigh << "]" << std::endl;
                    }
                    std::cout << "#> Random Seed: " << mRandGen.GetSeed() << std::endl;
                }
                for (int gen=0; gen<MaxGen; gen++) {
                    Evolve();
                    if (verbose) {
                        std::cout << "#----- Generation " << mGeneration << " -----#" << std::endl;
                        std::cout << "#> Best Fitness: " << mBestFitness << std::endl;
                    }
                    if (mCallback) { mCallback(*this); }
                    if (mEarlyStop) { 
                        if (mEarlyStop(*this)) {
                            if (verbose) {
                                std::cout << "#----- Eearly Stop: " << mGeneration << " generations with best fitness = " << mBestFitness << " -----#" << std::endl;
                            }
                            return;
                        }
                    }
                }
                if (verbose) {
                    std::cout << "#----- Normal End: " << mGeneration << " generations with best fitness = " << mBestFitness << " -----#" << std::endl;
                }
            }

            int GetGeneration() const { return mGeneration; }
            int GetMaxGeneration() const { return mMaxGeneration; }
            std::vector<std::vector<double>> GetPopulation() const { return mPopulation; }
            double GetBestFitness() { return mBestFitnessPerAgent.at(mBestIndex); }
            std::vector<double> GetBestAgent() { return mPopulation.at(mBestIndex); }
            bool IsValid(const std::vector<double>& features) {
                std::vector<Feature> constraints = mTarget.GetFeatures();
                // this features vector should have mD elements
                for (int i=0; i<mD; i++) {
                    double val = features.at(i);
                    if (!constraints.at(i).check(val)) {
                        return false;
                    }
                }
                return true;
            }

            std::function<void(const DiffEvolutionCore&)> mCallback;
            std::function<bool(const DiffEvolutionCore&)> mEarlyStop;
    };
}
#endif
