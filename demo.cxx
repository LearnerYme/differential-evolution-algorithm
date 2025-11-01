#include "DECore.h"
#include "FunctionTest.h"

#include "TCanvas.h"
#include "TF1.h"
#include "TGraph.h"
#include "TString.h"
#include "TH1D.h"
#include "TSystem.h"

static TCanvas c1("cas", "", 800, 600);

void MakeAgentsPlots(const mdea::DiffEvolutionCore& dea) {
    // note, there is only 1 dimension in the test function
    std::vector<std::vector<double>> population = dea.GetPopulation();
    int nAgents = population.size();
    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> tgs;
    for (auto agents : population) {
        double xx = agents[0];
        double yy = func.FitnessFunctionRaw(agents);
        x.push_back(xx);
        y.push_back(yy);
    }
    c1.Clear();
    c1.cd();
    TH1D* frame = (TH1D*)c1.DrawFrame(-0.5, -35, 20.5, 35);
    frame->SetTitle(Form("Generation %d of %d;;", dea.GetGeneration(), dea.GetMaxGeneration()));
    TF1* f1 = new TF1("f1", func.WrapperFunction, -0.5, 20.5);
    f1->SetLineColor(kBlack);
    TGraph* gr = new TGraph(nAgents, &x[0], &y[0]);
    gr->SetMarkerStyle(20);
    gr->SetMarkerColor(kRed);
    f1->Draw("same");
    gr->Draw("psame");
    
    
    if (gSystem->AccessPathName("DemoFigs")) {
        gSystem->mkdir("DemoFigs", true);
    }
    c1.Print(Form("DemoFigs/gen_%d.png", dea.GetGeneration()));
}


int main()
{
    mdea::DiffEvolutionCore dea(func, 20, 0.5, 0.5, 1.3);
    dea.mCallback = MakeAgentsPlots;
    dea.Main(100, true);
    return 0;
}