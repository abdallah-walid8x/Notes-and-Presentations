void macro3() {
    gStyle->SetOptStat(0);

    // --- Step 1: Generate simulated signal & background separately ---
    int nSignal = 10000;   // 10% of events
    int nBackground = 90000; // 90% of events

    TRandom3 rand(0);

    // Histograms
    TH1F* histSignal = new TH1F("histSignal", "Signal (Breit-Wigner);Mass [GeV/c^{2}];Entries", 100, 0, 2);
    TH1F* histBackground = new TH1F("histBackground", "Background (Gaussian);Mass [GeV/c^{2}];Entries", 100, 0, 2);

    // Fill signal dataset (Breit-Wigner)
    for (int i = 0; i < nSignal; ++i) {
        double mass = rand.BreitWigner(0.77, 0.15);
        histSignal->Fill(mass);
    }

    // Fill background dataset (Gaussian)
    for (int i = 0; i < nBackground; ++i) {
        double mass = rand.Gaus(0.5, 0.2);
        histBackground->Fill(mass);
    }

    // --- Step 2: Apply realistic error bars (stat + syst) ---
    for (int i = 1; i <= histSignal->GetNbinsX(); ++i) {
        double N = histSignal->GetBinContent(i);
        double statErr = sqrt(N);
        double systErr = 0.05 * N; // 5% systematic
        histSignal->SetBinError(i, sqrt(statErr*statErr + systErr*systErr));
    }
    for (int i = 1; i <= histBackground->GetNbinsX(); ++i) {
        double N = histBackground->GetBinContent(i);
        double statErr = sqrt(N);
        double systErr = 0.05 * N; // 5% systematic
        histBackground->SetBinError(i, sqrt(statErr*statErr + systErr*systErr));
    }

    // --- Step 3: Fit signal with Breit-Wigner ---
    TF1* fitBW = new TF1("fitBW", "breitwigner", 0, 2);
    fitBW->SetParameters(2000, 0.77, 0.15);
    histSignal->Fit(fitBW, "RQ");

    // --- Step 4: Fit background with Gaussian ---
    TF1* fitGaus = new TF1("fitGaus", "gaus", 0, 2);
    fitGaus->SetParameters(8000, 0.5, 0.2);
    histBackground->Fit(fitGaus, "RQ");

    cout << "Gaussian Chi2/NDF (background): " << fitGaus->GetChisquare()/fitGaus->GetNDF() << endl;
    cout << "Breit-Wigner Chi2/NDF (signal): " << fitBW->GetChisquare()/fitBW->GetNDF() << endl;

    // --- Step 5: Plot results ---
    TCanvas* c = new TCanvas("c", "Invariant Mass Analysis", 900, 700);

    histBackground->SetMarkerStyle(20);
    histBackground->SetMarkerColor(kBlue);
    histBackground->SetLineColor(kBlue);

    histSignal->SetMarkerStyle(21);
    histSignal->SetMarkerColor(kRed);
    histSignal->SetLineColor(kRed);

    histBackground->Draw("E1");
    histSignal->Draw("E1 SAME");

    fitGaus->SetLineColor(kBlue+2);
    fitGaus->SetLineWidth(2);
    fitBW->SetLineColor(kRed+2);
    fitBW->SetLineWidth(2);

    TLegend* leg = new TLegend(0.6, 0.7, 0.88, 0.88);
    leg->SetBorderSize(0);
    leg->AddEntry(histBackground, "Background (Gaussian)", "lep");
    leg->AddEntry(histSignal, "Signal (Breit-Wigner)", "lep");
    leg->AddEntry(fitGaus, "Gaussian Fit", "l");
    leg->AddEntry(fitBW, "Breit-Wigner Fit", "l");
    leg->Draw();

    // --- Step 6: Save outputs ---
    c->SaveAs("signal_background.pdf");
    c->SaveAs("signal_background.png");

    TFile outFile("analysis_output.root", "RECREATE");
    histSignal->Write();
    histBackground->Write();
    outFile.Close();

    cout << "Analysis complete. Separate signal and background stored." << endl;
}


