void macro2() {
    gStyle->SetOptStat(0);

    // --- Step 1: Generate simulated data and save to ROOT file ---
    std::string rootFile = "simulated_mass.root";
    TFile fOut(rootFile.c_str(), "RECREATE");
    TTree tree("events", "Simulated Invariant Mass Events");

    double mass;
    tree.Branch("mass", &mass);

    TRandom3 rand(0);
    int nEvents = 100000;

    for (int i = 0; i < nEvents; ++i) {
        if (rand.Rndm() < 0.1) { // 10% signal
            mass = rand.BreitWigner(0.77, 0.15);
        } else {
            mass = rand.Gaus(0.5, 0.2);
        }
        tree.Fill();
    }
    tree.Write();
    fOut.Close();

    cout << "Generated " << nEvents << " events into " << rootFile << endl;

    // --- Step 2: Read back the ROOT file and fill histogram ---
    TFile fIn(rootFile.c_str(), "READ");
    TTree* tIn = (TTree*)fIn.Get("events");
    if (!tIn) {
        std::cerr << "Error: could not find TTree 'events' in file " << rootFile << std::endl;
        return;
    }

    tIn->SetBranchAddress("mass", &mass);

    TH1F* hist = new TH1F("hist", "Invariant Mass Distribution;Mass [GeV/c^{2}];Entries", 100, 0, 2);
    for (Long64_t i = 0; i < tIn->GetEntries(); ++i) {
        tIn->GetEntry(i);
        hist->Fill(mass);
    }
    
    
     // --- Step 3: Fit Gaussian (background) ---
    TF1* fitGaus = new TF1("fitGaus", "gaus", 0, 2);
    fitGaus->SetParameters(100, 0.5, 0.2);
    hist->Fit(fitGaus, "Q");

    // --- Step 4: Fit Breit-Wigner (signal) ---
    TF1* fitBW = new TF1("fitBW", "breitwigner", 0, 2);
    fitBW->SetParameters(50, 0.77, 0.15);
    hist->Fit(fitBW, "Q+");

    cout << "Gaussian Chi2/NDF: " << fitGaus->GetChisquare()/fitGaus->GetNDF() << endl;
    cout << "Breit-Wigner Chi2/NDF: " << fitBW->GetChisquare()/fitBW->GetNDF() << endl;
    
    
    
    
    // --- Step 6: Plot results ---
    TCanvas* c = new TCanvas("c", "Invariant Mass Analysis", 800, 600);
    hist->SetMarkerStyle(20);
    hist->SetMarkerSize(0.8);
    hist->Draw("E1");

    hist->GetFunction("fitGaus")->SetLineColor(kBlue);
    hist->GetFunction("fitBW")->SetLineColor(kRed);

    TLegend* leg = new TLegend(0.65, 0.7, 0.88, 0.88);
    leg->SetBorderSize(0);
    leg->AddEntry(hist, "Simulated Data", "lep");
    leg->AddEntry("fitGaus" , "Gaussian Background", "l");
    leg->AddEntry("fitBW" , "Breit-Wigner Signal" ,"l");
    leg->Draw();
    
    

   


    // --- Step 5: Save outputs ---
    
    
    c->SaveAs("invariant_mass.pdf");
    c->SaveAs("invariant_mass.png");

    TFile outFile("analysis_output.root", "RECREATE");
    hist->Write();
    outFile.Close();

    cout << "Analysis complete. Outputs saved." << endl;
    

    
}

