void DrawWaveform(int id = 467)
{
    int CHid1 = 2;
    int CHid2 = 4;
    char buff[1024];
    const char *name = "testbeam_dumpfile.root";
    TFile *f1 = new TFile(name,"read");
    sprintf(buff,"CH%dgraph_%d",CHid1,id);
TGraph *g1 = (TGraph*)f1->Get(buff);
    sprintf(buff,"CH%dgraph_%d",CHid2,id);
TGraph *g2 = (TGraph*)f1->Get(buff);

    g1->SetLineWidth(3);
    g2->SetLineWidth(3);
    g2->SetLineColor(2);

    g1->GetXaxis()->SetRangeUser(72,82);
    g1->Draw();
    g2->Draw("same");
}