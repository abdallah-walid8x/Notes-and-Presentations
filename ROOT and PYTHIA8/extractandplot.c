void extractandplot()
{
    
    //Create histogram and random number generator
    TH1F *hist = new TH1F("hist", "", 100, 0, 15);   // Histogram with 100 bins, range 0–15
    TRandom2 *rand = new TRandom2(3);                // Random generator with seed=3
    fstream file;

    // Generate random Gaussian-distributed data and save to file
    file.open("data.txt", ios::out);
    for (int i = 0; i < 1000; i++)
    {
        double r = rand->Gaus(5, 1);   // Gaussian with mean=5, sigma=1
        file << r << endl;             // Write values to file
    }
    file.close();

 
    // Read data back from file and fill histogram
    file.open("data.txt", ios::in);
    double value;
    while (1)
    {
        file >> value;
        hist->Fill(value);             // Fill histogram with each value
        if (file.eof()) break;         // Stop at end of file
    }
    file.close();

    // Customize histogram appearance
    hist->SetFillColor(kGreen - 9);
    hist->GetXaxis()->SetTitle("Distribution");
    hist->GetYaxis()->SetTitle("Entries");
    hist->GetXaxis()->SetTitleSize(0.05);
    hist->GetYaxis()->SetTitleSize(0.05);
    hist->GetXaxis()->SetLabelSize(0.05);
    hist->GetYaxis()->SetLabelSize(0.05);


    // Define Gaussian fit function with initial parameters
    TF1 *fit = new TF1("fit", "gaus", 0, 15);
    fit->SetLineWidth(3);
    fit->SetLineColor(kRed);
    fit->SetLineStyle(2);
    fit->SetParameter(0, 40);   // Normalization (initial guess)
    fit->SetParameter(1, 5);    // Mean (initial guess)
    fit->SetParameter(2, 1);    // Sigma (initial guess)


    // Create canvas, enable grid, fit histogram with Gaussian, and draw
    TCanvas *c1 = new TCanvas();
    c1->SetTickx();
    c1->SetTicky();
    c1->SetGridx();
    c1->SetGridy();
    hist->Fit("fit");   // Perform Gaussian fit
    hist->Draw();       // Draw histogram

    // Add legend to distinguish data and fit
    TLegend *leg = new TLegend(0.5, 0.55, 0.8, 0.75);
    leg->SetBorderSize(0);
    leg->AddEntry(hist, "Measured Data", "p");
    leg->AddEntry(fit, "Fit Function", "l");
    leg->Draw();

    // Draw reference line and annotation arrow
    TLine *l = new TLine(0, 20, 15, 20);   // Horizontal line at y=20
    l->SetLineWidth(2);
    l->SetLineColor(kOrange);
    l->Draw();

    //Arrow pointing to a specific histogram bin
    double x0 = 6.3;
    int bin = hist->FindBin(x0);
    double y0 = hist->GetBinContent(bin);
    TArrow *arr = new TArrow(10, 30, x0, y0);
    arr->SetLineWidth(2);
    arr->SetArrowSize(0.02);
    arr->SetLineColor(kBlue);
    arr->Draw();

    //Text label near the arrow
    TLatex *t = new TLatex(10, 30, "Important");
    t->Draw();


    //Extract fit parameters and compute mean/sigma ratio
    double mean  = fit->GetParameter(1);
    double sigma = fit->GetParameter(2);
    cout << mean / sigma << endl;   // Print ratio to console
}

