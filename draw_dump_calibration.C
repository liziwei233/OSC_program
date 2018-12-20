{
    gROOT->Reset();
file0 =  new TFile("calibration_dumpfile.root");
file0->ls();

//n =  new (int);
//*n=file0->Get("n")->GetUniqueID();

//N = new int;
//*N = file0->Get("N")->GetUniqueID();

TCanvas c0("MM1","MM1");
unfiltered_graph_0->SetTitle("MM1");
unfiltered_graph_0->Draw("A*");
TGraph nofftgr = *graph_0;
nofftgr.SetLineColor(kBlue);
nofftgr.SetLineWidth(2.);
nofftgr.Draw("L");
rawsigmoid->Draw("same");


}
