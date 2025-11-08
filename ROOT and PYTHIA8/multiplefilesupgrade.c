#include <TRandom3.h>
#include <TStyle.h>
#include <TLegend.h>
#include <THStack.h>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TFile.h>
#include <iostream>
#include <fstream>

void multiplefilesupgrade() {
    gStyle->SetOptStat(0);

    const int nHists = 6;            // number of histograms per file
    const int nFiles = 10;           // total number of files
    const int nEventsPerFile = 1000; // events per file

    TRandom3 randGen(0); // random seed for reproducibility

    // Parameters for each dataset's Gaussian (mean, sigma)
    double means[nHists]  = {0.0, 0.5, -0.5, 1.0, -1.0, 0.0};
    double sigmas[nHists] = {1.0, 0.8, 1.2, 0.6, 1.5, 2.0};

    TH1F *hist[nHists];
    THStack *hstack = new THStack("stack", "Momentum Distributions;P [GeV/c];Entries");
    TLegend *leg = new TLegend(0.65, 0.5, 0.88, 0.88);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);

    // --- Create histograms ---
    for (Int_t i = 0; i < nHists; i++) {
        TString name = Form("hist%d", i);
        hist[i] = new TH1F(name, "", 100, -5, 5);
        hist[i]->SetMarkerStyle(20 + i);
        hist[i]->SetMarkerSize(0.8);
        hist[i]->SetMarkerColor(i + 1);
        hist[i]->SetLineColor(i + 1);
        hstack->Add(hist[i], "E1");
        leg->AddEntry(hist[i], Form("Dataset %d", i + 1), "lep");
    }

    // --- Generate + Read data ---
    for (Int_t iFile = 0; iFile < nFiles; iFile++) {
        TString filename = Form("input%d", iFile);

        // Generate synthetic data with different means/sigmas per histogram
        {
            std::ofstream outfile(filename.Data());
            if (!outfile.is_open()) {
                std::cerr << "Error: cannot write " << filename << std::endl;
                continue;
            }

            for (int ev = 0; ev < nEventsPerFile; ev++) {
                for (int j = 0; j < nHists; j++) {
                    double val = randGen.Gaus(means[j], sigmas[j]);
                    outfile << val << " ";
                }
                outfile << "\n";
            }
            outfile.close();
        }

        // Read data back into histograms
        std::ifstream infile(filename.Data());
        if (!infile.is_open()) {
            std::cerr << "Warning: could not open " << filename << " for reading." << std::endl;
            continue;
        }

        while (true) {
            for (Int_t j = 0; j < nHists; j++) {
                Double_t val;
                infile >> val;
                if (!infile) break;
                hist[j]->Fill(val);
            }
            if (infile.eof()) break;
        }
        infile.close();
    }

    // --- Fit each histogram & print parameters ---
    std::cout << "\n===== Gaussian Fit Results =====" << std::endl;
    TF1 *fitFunc[nHists];
    for (Int_t i = 0; i < nHists; i++) {
        fitFunc[i] = new TF1(Form("fit%d", i), "gaus", -5, 5);
        fitFunc[i]->SetLineColor(hist[i]->GetLineColor());
        hist[i]->Fit(fitFunc[i], "RQ"); // RQ = quiet fit, but returns result

        double mean  = fitFunc[i]->GetParameter(1);
        double sigma = fitFunc[i]->GetParameter(2);
        double meanErr  = fitFunc[i]->GetParError(1);
        double sigmaErr = fitFunc[i]->GetParError(2);
        double chi2ndf  = fitFunc[i]->GetChisquare() / fitFunc[i]->GetNDF();

        std::cout << Form("Dataset %d:", i + 1)
                  << " Mean = " << mean << " ± " << meanErr
                  << " GeV/c, Sigma = " << sigma << " ± " << sigmaErr
                  << " GeV/c, Chi²/NDF = " << chi2ndf
                  << ", Entries = " << hist[i]->GetEntries()
                  << std::endl;
    }

    // --- Draw ---
    TCanvas *c1 = new TCanvas("c1", "Momentum Distributions", 1000, 800);
    c1->SetGrid();
    c1->SetTickx();
    c1->SetTicky();
    hstack->Draw("nostack E1");
    for (int i = 0; i < nHists; i++) {
        fitFunc[i]->Draw("same");
    }
    hstack->GetXaxis()->SetTitleSize(0.045);
    hstack->GetYaxis()->SetTitleSize(0.045);
    hstack->GetXaxis()->SetLabelSize(0.04);
    hstack->GetYaxis()->SetLabelSize(0.04);
    leg->Draw();

    // --- Save ---
    c1->SaveAs("momentum_distributions_with_fits.pdf");
    c1->SaveAs("momentum_distributions_with_fits.png");

    TFile outFile("momentum_distributions_with_fits.root", "RECREATE");
    for (Int_t i = 0; i < nHists; i++) {
        hist[i]->Write();
        fitFunc[i]->Write();
    }
    outFile.Close();

    std::cout << "\nAnalysis complete. Fits overlaid, parameters printed above, ROOT & plots saved." << std::endl;
}


