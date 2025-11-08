#include "Pythia8/Pythia.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TStyle.h"
#include <cmath>

int main() {
    // --- Settings ---
    int nevents = 5000;   // Number of events
    double eCM = 200.0;     // RHIC energy (GeV) for STAR pp collisions

    // --- Initialize Pythia ---
    Pythia8::Pythia pythia;
    pythia.readString("Beams:idA = 2212");
    pythia.readString("Beams:idB = 2212");
    pythia.readString("Beams:eCM = 200."); // STAR pp energy
    //pythia.readString("SoftQCD:inelastic = on");
    pythia.readString("HardQCD:all = on");
    pythia.init();

    // --- ROOT histograms ---
    TH1F *h_pT  = new TH1F("h_pT",  ";p_{T} [GeV/c];Entries", 100, 0, 5);
    TH1F *h_eta = new TH1F("h_eta", ";#eta;Entries", 100, -5, 5);
    TH1F *h_phi = new TH1F("h_phi", ";#phi [rad];Entries", 64, -M_PI, M_PI);
    TH2F *h_pT_eta = new TH2F("h_pT_eta", ";#eta;p_{T} [GeV/c]", 50, -2.5, 2.5, 50, 0, 5);

    h_pT->Sumw2();
    h_eta->Sumw2();
    h_phi->Sumw2();
    h_pT_eta->Sumw2();

    // --- Event loop ---
    for (int i = 0; i < nevents; i++) {
        if (!pythia.next()) continue;

        for (int j = 0; j < pythia.event.size(); j++) {
            if (!pythia.event[j].isFinal()) continue; // Final state only

            double pT  = pythia.event[j].pT();
            double eta = pythia.event[j].eta();
            double phi = pythia.event[j].phi();

            // STAR acceptance cut
            if (fabs(eta) < 1.0 && pT > 0.2) {
                h_pT->Fill(pT);
                h_eta->Fill(eta);
                h_phi->Fill(phi);
            }

            // Fill 2D histogram without acceptance cut to show full coverage
            h_pT_eta->Fill(eta, pT);
        }
    }

    // --- Save histograms to ROOT file ---
    TFile outFile("pythia_histograms.root", "RECREATE");
    h_pT->Write();
    h_eta->Write();
    h_phi->Write();
    h_pT_eta->Write();
    outFile.Close();

    // --- STAR-style plotting ---
    gStyle->SetOptStat(0);
    gStyle->SetTitleFontSize(0.05);

    // pT plot
    TCanvas *c1 = new TCanvas("c1", "pT Distribution", 800, 600);
    c1->SetLogy();
    h_pT->SetMarkerStyle(20);
    h_pT->Draw("E1");
    c1->SaveAs("pT_distribution.pdf");
    c1->SaveAs("pT_distribution.png");

    // eta plot
    TCanvas *c2 = new TCanvas("c2", "Eta Distribution", 800, 600);
    h_eta->SetMarkerStyle(20);
    h_eta->Draw("E1");
    c2->SaveAs("eta_distribution.pdf");
    c2->SaveAs("eta_distribution.png");

    // phi plot
    TCanvas *c3 = new TCanvas("c3", "Phi Distribution", 800, 600);
    h_phi->SetMarkerStyle(20);
    h_phi->Draw("E1");
    c3->SaveAs("phi_distribution.pdf");
    c3->SaveAs("phi_distribution.png");

    // 2D pT vs eta plot
    TCanvas *c4 = new TCanvas("c4", "pT vs Eta", 900, 700);
    c4->SetRightMargin(0.15);
    gStyle->SetPalette(kBird);
    h_pT_eta->Draw("COLZ");
    c4->SaveAs("pT_vs_eta.pdf");
    c4->SaveAs("pT_vs_eta.png");

    // --- Print Pythia statistics ---
    pythia.stat();

    return 0;
}

