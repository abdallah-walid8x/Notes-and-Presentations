void extract2(const char* infile = "simulated_mass.txt") {
    // --- Settings ---
    const int nBins = 100;
    const double minMass = 2.0;   // GeV/c^2
    const double maxMass = 4.0;   // GeV/c^2

    // --- Histogram ---
    TH1F *hMass = new TH1F("hMass", ";Invariant Mass M_{#mu#mu} (GeV/c^{2});Entries / bin", nBins, minMass, maxMass);
    hMass->SetMarkerStyle(20);
    hMass->SetMarkerSize(1.0);

    // --- Random data simulation (replace later with real data reading) ---
    TRandom3 rand(42);
    std::ofstream fout(infile);
    const double signalYield = 500;
    const double bkgYield = 2000;

    // Signal: Gaussian centered at 3.1 GeV/c^2 (J/psi mass)
    for (int i = 0; i < signalYield; i++) {
        double mass = rand.Gaus(3.097, 0.05); // mean, sigma
        fout << mass << "\n";
    }
    // Background: exponential falloff
    for (int i = 0; i < bkgYield; i++) {
        double mass = minMass - log(rand.Uniform()) * 0.8; // lambda
        if (mass < maxMass) fout << mass << "\n";
    }
    fout.close();

    // --- Read data ---
    std::ifstream fin(infile);
    double val;
    while (fin >> val) {
        if (val >= minMass && val <= maxMass) hMass->Fill(val);
    }
    fin.close();

    // --- Fit function: Gaussian + exponential background ---
    TF1 *fitFunc = new TF1("fitFunc", "[0]*exp([1]*x) + [2]*exp(-0.5*((x-[3])/[4])**2)", minMass, maxMass);
    fitFunc->SetParameters(100, -1, 200, 3.1, 0.05); // initial guesses
    fitFunc->SetParNames("BkgNorm", "BkgSlope", "SigNorm", "SigMean", "SigSigma");

    // --- Canvas ---
    TCanvas *c1 = new TCanvas("c1", "STAR-style Analysis", 800, 600);
    gStyle->SetOptFit(1111);
    gStyle->SetStatX(0.88);
    gStyle->SetStatY(0.88);

    hMass->Draw("E1");
    hMass->Fit("fitFunc", "R");

    // --- Legend ---
    TLegend *leg = new TLegend(0.2, 0.55, 0.48, 0.75);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->AddEntry(hMass, "Simulated Data", "lep");
    leg->AddEntry(fitFunc, "Fit: Gauss + Exp", "l");
    leg->Draw();

    // --- Annotation ---
    TLatex text;
    text.SetNDC();
    text.SetTextSize(0.04);
    text.DrawLatex(0.15, 0.85, "#bf{STAR Simulation}");
    text.DrawLatex(0.15, 0.80, "p+p #sqrt{s} = 200 GeV");

    // --- Save outputs ---
    c1->SaveAs("mass_spectrum.pdf");
    c1->SaveAs("mass_spectrum.png");

    // --- Save histogram and fit to ROOT file ---
    TFile outFile("analysis_results.root", "RECREATE");
    hMass->Write();
    fitFunc->Write();
    outFile.Close();
}

