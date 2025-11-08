void two_d_histogram() {
    // --- Settings ---
    const int nEvents = 1e6;
    const double meanVal = 0.0;
    const double sigmaVal = 1.0;

    // --- Create histograms ---
    TH2F *h2 = new TH2F("h2", ";x [cm];y [cm]", 200, -3, 3, 200, -3, 3);

    // Random generator
    TRandom3 rand(10);

    // Fill 2D Gaussian
    for (int i = 0; i < nEvents; i++) {
        double x = rand.Gaus(meanVal, sigmaVal);
        double y = rand.Gaus(meanVal, sigmaVal);
        h2->Fill(x, y);
    }

    // --- Create canvas with pads for projections ---
    TCanvas *c = new TCanvas("c", "STAR-style 2D Gaussian with Projections", 1000, 1000);
    gStyle->SetOptStat(0);
    gStyle->SetPalette(kBird);
    gStyle->SetNumberContours(100);

    // Pads
    TPad *padMain = new TPad("padMain", "Main 2D", 0.0, 0.0, 0.8, 0.8);
    TPad *padX    = new TPad("padX", "X projection", 0.0, 0.8, 0.8, 1.0);
    TPad *padY    = new TPad("padY", "Y projection", 0.8, 0.0, 1.0, 0.8);

    padMain->SetRightMargin(0.15);
    padMain->SetLeftMargin(0.15);
    padMain->SetBottomMargin(0.15);
    padX->SetBottomMargin(0.0);
    padX->SetLeftMargin(0.15);
    padX->SetRightMargin(0.15);
    padY->SetTopMargin(0.15);
    padY->SetLeftMargin(0.0);

    padMain->Draw();
    padX->Draw();
    padY->Draw();

    // --- Draw main 2D plot ---
    padMain->cd();
    h2->Draw("COLZ");
    h2->Draw("CONT3 SAME"); // add contour lines on top

    // --- X projection ---
    padX->cd();
    TH1D *hX = h2->ProjectionX();
    hX->SetLineColor(kBlue+1);
    hX->SetFillColorAlpha(kBlue, 0.3);
    hX->GetXaxis()->SetLabelSize(0);
    hX->GetYaxis()->SetTitle("Entries");
    hX->Draw();

    // --- Y projection ---
    padY->cd();
    TH1D *hY = h2->ProjectionY();
    hY->SetLineColor(kRed+1);
    hY->SetFillColorAlpha(kRed, 0.3);
    hY->GetYaxis()->SetLabelSize(0);
    hY->Draw("HIST");
    hY->SetMaximum(hY->GetMaximum() * 1.1);

    // --- Output statistics to terminal ---
    double meanX = h2->GetMean(1);
    double meanY = h2->GetMean(2);
    double sigmaX = h2->GetRMS(1);
    double sigmaY = h2->GetRMS(2);

    std::cout << "===== STAR-style 2D Gaussian Analysis =====" << std::endl;
    std::cout << "Number of entries: " << h2->GetEntries() << std::endl;
    std::cout << "Mean X = " << meanX << " cm" << std::endl;
    std::cout << "Sigma X = " << sigmaX << " cm" << std::endl;
    std::cout << "Mean Y = " << meanY << " cm" << std::endl;
    std::cout << "Sigma Y = " << sigmaY << " cm" << std::endl;

    // --- Save outputs ---
    c->SaveAs("beamspot_2D_with_projections.pdf");
    c->SaveAs("beamspot_2D_with_projections.png");

    TFile outFile("beamspot_2D_with_projections.root", "RECREATE");
    h2->Write();
    hX->Write("projX");
    hY->Write("projY");
    outFile.Close();
}

