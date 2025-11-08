void view2_3d() {
    gStyle->SetOptStat(0);

    // --- Canvas ---
    TCanvas *c1 = new TCanvas("c1", "STAR-style 3D Geometry", 1200, 900);

    // --- Geometry Manager ---
    TGeoManager *man = new TGeoManager("STARgeom", "STAR Detector Geometry");

    // --- Materials and Mediums ---
    TGeoMaterial *matVacuum = new TGeoMaterial("Vacuum", 0, 0, 0);
    TGeoMedium   *vacuum    = new TGeoMedium("Vacuum", 1, matVacuum);

    TGeoMaterial *matAl = new TGeoMaterial("Aluminum", 26.98, 13, 2.7);
    TGeoMedium   *alum  = new TGeoMedium("Aluminum", 2, matAl);

    TGeoMaterial *matSi = new TGeoMaterial("Silicon", 28.085, 14, 2.33);
    TGeoMedium   *silicon = new TGeoMedium("Silicon", 3, matSi);

    // --- World volume ---
    TGeoVolume *top = man->MakeBox("TOP", vacuum, 50, 50, 50);
    man->SetTopVolume(top);

    // --- Barrel detector (tube) ---
    TGeoVolume *barrel = man->MakeTube("TPC", silicon, 20.0, 22.0, 40.0);
    barrel->SetLineColor(kBlue - 7);
    barrel->SetTransparency(60);
    top->AddNode(barrel, 0);

    // --- Endcap disk ---
    TGeoVolume *endcap = man->MakeTube("Endcap", alum, 0.0, 22.0, 1.0);
    endcap->SetLineColor(kGreen + 2);
    endcap->SetTransparency(40);
    top->AddNode(endcap, 0, new TGeoTranslation(0, 0, 40.0));
    top->AddNode(endcap, 1, new TGeoTranslation(0, 0, -40.0));

    // --- Additional inner tracker layer ---
    TGeoVolume *innerLayer = man->MakeTube("InnerTracker", silicon, 5.0, 5.5, 40.0);
    innerLayer->SetLineColor(kOrange + 7);
    innerLayer->SetTransparency(40);
    top->AddNode(innerLayer, 0);

    // --- Close geometry ---
    man->CloseGeometry();

    // --- Draw geometry ---
    top->Draw("ogl");

    // --- Access 3D viewer ---
    TGLViewer *view = (TGLViewer*)gPad->GetViewer3D();
    if (view) {
        view->SetStyle(TGLRnrCtx::kOutline); // Outline mode for clarity
        view->CurrentCamera().RotateRad(-0.5, 0.5); // nice angle
        view->RequestDraw();
    }

    // --- Add multiple tracks ---
    TRandom3 r(0);
    for (int t = 0; t < 5; t++) {
        TPolyLine3D *track = new TPolyLine3D();
        track->SetLineColor(kRed + (t % 3));
        track->SetLineWidth(2);
        double phi0 = r.Uniform(0, 2 * TMath::Pi());
        double radius = r.Uniform(5.0, 20.0);
        double zpos = -40.0;
        for (int p = 0; p < 100; p++) {
            double angle = phi0 + p * 0.02;
            double x = radius * cos(angle);
            double y = radius * sin(angle);
            double z = zpos + p * (80.0 / 100);
            track->SetPoint(p, x, y, z);
        }
        track->Draw("same");
    }

    // --- Annotation ---
    TLatex label;
    label.SetNDC();
    label.SetTextSize(0.035);
    label.DrawLatex(0.02, 0.96, "#bf{STAR Detector Geometry - Simulation}");
    label.DrawLatex(0.02, 0.92, "TPC + Endcaps + Inner Tracker");

    // --- Legend for materials ---
    TLegend *leg = new TLegend(0.7, 0.7, 0.9, 0.88);
    leg->AddEntry(barrel, "TPC Barrel (Si)", "f");
    leg->AddEntry(endcap, "Endcaps (Al)", "f");
    leg->AddEntry(innerLayer, "Inner Tracker (Si)", "f");
    leg->Draw();

    // --- Save outputs ---
    c1->SaveAs("star_geometry_full.png");
    c1->SaveAs("star_geometry_full.pdf");

    std::cout << "3D STAR-style geometry saved as PNG and PDF.\n";
}

