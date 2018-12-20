{
    gROOT->Reset();
file0 =  new TFile("dumpfile.root");
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

TCanvas c1("MCP1","MCP1");
graph_1->SetTitle("MCP1");
graph_1->Draw("A*");

TCanvas c2("MCP2","MCP2");
graph_2->SetTitle("MCP2");
graph_2->Draw("A*");

}
