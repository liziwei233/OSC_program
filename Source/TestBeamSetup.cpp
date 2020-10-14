#include "TestBeamSetup.h"
#include <iostream>
#include "TLine.h"
#include "TVirtualPad.h"

TestBeamSetup::~TestBeamSetup()
{
    if(!avers.empty()){

    for(int i = 0 ; i < avers.size(); ++i)
        delete avers.at(i);
    }
}
void TestBeamSetup::TestBeamAnalysis()
{
    ScaleAndShiftTimes();
    for (int i = 0; i < NofDetectors; ++i)
    {
        Detectors.at(i)->InvertY();
    }
    //Dynamically find the end for the baseline calculation region
    //for(int i = 0; i < NofDetectors; ++i)
    //{
    //}
    //===
    for (int i = 0; i < NofDetectors; ++i)
    {
        baseline_region_end = 2000000;
        Detectors.at(i)->FindGlobalMaximum(0, Detectors.at(i)->waveform_y.size() - 1);
        if (Detectors.at(i)->global_maximum.position < baseline_region_end)
            baseline_region_end = Detectors.at(i)->global_maximum.position;
        //std::cout<<"NofDet="<<i<<",NofPoint="<<Detectors.at(i)->waveform_y.size()<<",baseline_region_end="<<baseline_region_end-200<<std::endl;

        baseline_region_end -= 400;
        if (baseline_region_end < 0)
        {

            baseline_region_end = 100.;
            if (baseline_region_end > Detectors.at(i)->global_maximum.position)
            {
                baseline_region_end = 50.;
                if (baseline_region_end > Detectors.at(i)->global_maximum.position)
                    baseline_region_end = 25.;
            }
        }
        record_blregion_end[i] = baseline_region_end;
        max_region_end = 2000 + baseline_region_end;

        Detectors.at(i)->SubstractBaseline(baseline_region_end);

        //Detectors.at(i)->FindGlobalMaximum(baseline_region_end, max_region_end);
        Detectors.at(i)->FindFirstPeak(baseline_region_end, max_region_end);
        Detectors.at(i)->ConvertFirstPeak2GlobalMaximum();
        Detectors.at(i)->FindStartPoint(baseline_region_end);
        Detectors.at(i)->FindEndPoint(max_region_end);
        Detectors.at(i)->CalculateCharges();
        Detectors.at(i)->FindRiseTime();
        Detectors.at(i)->FindWidth();
        Detectors.at(i)->FindInvertMaximum(baseline_region_end, Detectors.at(i)->global_maximum.position);
        Detectors.at(i)->FindSecondInvertPeak(Detectors.at(i)->global_maximum.position);
        //Detectors.at(i)->FindElectronPeakEndPoint();
        // Detectors.at(i)->FindNaiveTiming();
        //Detectors.at(i)->FindFirstPeak();
        //Detectors.at(i)->FindMaxDerivative();

        if (Detectors.at(i)->type == 0)
            Detectors.at(i)->TimeInformation(0, 1, 7);
        else
        {
            Detectors.at(i)->TimeInformation(0, 0, 1);
        }

        if (Detectors.at(i)->type > 0) //for MM
        {

            Detectors.at(i)->TimeSigmoid();
            Detectors.at(i)->FilterWaveformFFT(baseline_region_end, 8192, 2.5);
            Detectors.at(i)->FindGlobalMaximum(0, 8191);
            Detectors.at(i)->FindStartPoint(0);
            Detectors.at(i)->FindEndPoint(max_region_end - baseline_region_end);
            Detectors.at(i)->FindElectronPeakEndPoint();
            Detectors.at(i)->CalculateCharges();
            Detectors.at(i)->FindNaiveTiming();
            Detectors.at(i)->FindRiseTime();
            //Detectors.at(i)->FindFirstPeak();
            Detectors.at(i)->FindMaxDerivative();
            Detectors.at(i)->TimeSigmoid();
        }
        //Detectors.at(i)->TimeInflection();
        //Detectors.at(i)->TimeTwentyPercent();
    }
}

void TestBeamSetup::Dump(int id)
//void TestBeamSetup::Dump(int id)
{
    TFile dumpfile("testbeam_dumpfile.root", "update");
    //TTree *newtree = OutTree->CloneTree(0);
    //newtree->Fill();

    TObject integer;

    integer.SetUniqueID(Detectors.size());
    integer.Write("n");

    TGraph gr;
    TGraph gMP;  // main peak
    TGraph gCTP; //crosstalk peak
    TGraph gRP;  // ringing peak
    for (int i = 0; i < Detectors.size(); ++i)
    {
        char str1[20];
        sprintf(str1, "CH%dgraph_%d", Channel_IDs.at(i), id);
        gr = TGraph(Detectors.at(i)->waveform_x.size(), &Detectors.at(i)->waveform_x[0], &Detectors.at(i)->waveform_y[0]);
        gr.Write(str1);

        gMP.SetPoint(0, Detectors.at(i)->global_maximum.x, Detectors.at(i)->global_maximum.y);
        gCTP.SetPoint(0, Detectors.at(i)->invert_maximum.x, Detectors.at(i)->invert_maximum.y);
        gRP.SetPoint(0, Detectors.at(i)->SecondInvertPeak.x, Detectors.at(i)->SecondInvertPeak.y);
        //sprintf(str1,"graph_%d",i,id);
        gMP.SetMarkerSize(2);
        gMP.SetMarkerStyle(32);
        gMP.SetMarkerColor(3);
        gCTP.SetMarkerSize(2);
        gCTP.SetMarkerStyle(46);
        gCTP.SetMarkerColor(2);
        gRP.SetMarkerSize(2);
        gRP.SetMarkerStyle(41);
        gRP.SetMarkerColor(2);

        if (Detectors.at(i)->pre_filter_backup)
        {
            gr = TGraph(Detectors.at(i)->pre_filter_backup->waveform_x.size(), &Detectors.at(i)->pre_filter_backup->waveform_x[0], &Detectors.at(i)->pre_filter_backup->waveform_y[0]);
            sprintf(str1, "unfiltered_graph_%d_%d", i, id);
            //sprintf(str1,"unfiltered_graph_%d_%d",i,id);
            gr.Write(str1);
        }

        gr.Draw("AL");
        gMP.Draw("Psame");
        gCTP.Draw("Psame");
        gRP.Draw("Psame");
        TLine *linebl = new TLine(Detectors.at(i)->waveform_x[0], 0, Detectors.at(i)->waveform_x.at(record_blregion_end[i]), 0);
        linebl->SetLineColor(2);
        linebl->SetLineWidth(2);
        linebl->Draw();
        TLine *lineQ = new TLine(Detectors.at(i)->start_point.x, Detectors.at(i)->global_maximum.y, Detectors.at(i)->end_point.x, Detectors.at(i)->global_maximum.y);
        lineQ->SetLineColor(2);
        lineQ->SetLineWidth(2);
        lineQ->Draw();
        char str2[20];
        sprintf(str2, "CH%dC_%d", Channel_IDs.at(i), id);
        gPad->Write(str2);
    }

    //Detectors.at(0)->pre_filter_backup->Sigmoid.fit_func.Write("rawsigmoid");
    //newtree->Write();
    dumpfile.Close();
}

//void TestBeamSetup::SetWaveformToAverage(AverageTool &aver)
//{
//    ScaleAndShiftTimes();
//    for(int i = 0; i < NofDetectors; ++i)
//    {
//        if( Detectors.at(i)->type > 0 )
//            Detectors.at(i)->InvertY();
//    }
//
//    baseline_region_end=2000000;
//    for(int i = 0; i < NofDetectors; ++i)
//    {
//        Detectors.at(i)->FindGlobalMaximum(0, Detectors.at(i)->waveform_y.size()-1);
//        if(Detectors.at(i)->global_maximum.position < baseline_region_end)
//            baseline_region_end = Detectors.at(i)->global_maximum.position;
//    }
//    baseline_region_end-=200;
//    if(baseline_region_end < 2) baseline_region_end = 2;
//    //===
//    //
//    int j = 0;
//    double ref_time=0;
//    double n = 0;
//    for(int i = 0; i < NofDetectors; ++i)
//    {
//        Detectors.at(i)->SubstractBaseline(baseline_region_end);
//        if( Detectors.at(i)->type > 0 )
//        {
//            j = i;
//            Detectors.at(i)->FilterWaveformFFT(0, 4096, 1.0);
//            Detectors.at(i)->FindGlobalMaximum(0, 4095);
//            Detectors.at(i)->FindStartPoint(baseline_region_end);
//            Detectors.at(i)->FindEndPoint(max_region_end-baseline_region_end);
//            Detectors.at(i)->FindElectronPeakEndPoint();
//            Detectors.at(i)->CalculateCharges();
//            Detectors.at(i)->FindNaiveTiming();
//        }
//        else
//        {
//            Detectors.at(i)->FindGlobalMaximum(0, Detectors.at(i)->waveform_y.size()-1);
//            Detectors.at(i)->FindStartPoint(baseline_region_end);
//            Detectors.at(i)->FindEndPoint(max_region_end-baseline_region_end);
//            Detectors.at(i)->FindNaiveTiming();
//            Detectors.at(i)->TimeTwentyPercent();
//            Detectors.at(i)->FindFirstPeak();
//            Detectors.at(i)->FindMaxDerivative();
//            Detectors.at(i)->TimeInflection();
//            //ref_time += Detectors.at(i)->Inflection.timing;
//            ref_time += Detectors.at(i)->TwentyPercent.x;
//            n++;
//        }
//    }
//
//    ref_time/= n;
//
//    double normalization = 1./Detectors.at(j)->global_maximum.y;
//    double normalization2 = 1./Detectors.at(j)->charge_e_peak;
//    //double normalization = 0;
//    //for(int i = 0; i < Detectors.at(j)->waveform_y.size(); ++i)
//    //{
//    //    normalization += Detectors.at(j)->waveform_y.at(i);
//    //}
//    //normalization /= Detectors.at(j)->waveform_y.size();
//
//    //double ph = Detectors.at(j)->charge_e_peak;
//    double tt = Detectors.at(0)->naive_time - Detectors.at(1)->TwentyPercent.x;
//    double ph = Detectors.at(j)->global_maximum.y;
//    double ch = Detectors.at(j)->charge_e_peak;
//    //if (ph <0.3|| ph>=0.42) baseline_region_end = -1;
//    if (ch < 1.9|| ch>= 2.2) baseline_region_end = -1;
//    //if (tt<7.5) baseline_region_end = -1;
//    aver.SetWaveform(Detectors.at(j)->waveform_x, Detectors.at(j)->waveform_y, ref_time, normalization, baseline_region_end, normalization2);
//    //aver.SetWaveform(Detectors.at(j)->waveform_x, Detectors.at(j)->waveform_y, ref_time, normalization, baseline_region_end);
//}
void TestBeamSetup::CreateAverageTools()
{
    for(int i =0; i<NofDetectors;++i)
    {
        avers.push_back(new AverageTool());
    }

}


void TestBeamSetup::SetWaveformToAverage()
{
    ScaleAndShiftTimes();
    for (int i = 0; i < NofDetectors; ++i)
    {
        Detectors.at(i)->InvertY();
    }
    baseline_region_end = 2000000;
    int j = 0;
    double ref_time = 0;
    double n = 0;
    double normalization = 0;
    for (int i = 0; i < NofDetectors; ++i)
    {
        normalization = 1;
        Detectors.at(i)->FindGlobalMaximum(0, Detectors.at(i)->waveform_y.size() - 1);
        if (Detectors.at(i)->global_maximum.position < baseline_region_end)
            baseline_region_end = Detectors.at(i)->global_maximum.position;

        baseline_region_end -= 400;
        if (baseline_region_end < 0)
        {

            baseline_region_end = 100.;
            if (baseline_region_end > Detectors.at(i)->global_maximum.position)
            {
                baseline_region_end = 50.;
                if (baseline_region_end > Detectors.at(i)->global_maximum.position)
                    baseline_region_end = 25.;
            }
        }
        max_region_end = 2000 + baseline_region_end;
        //===
        //

        if (Detectors.at(i)->type > 0)
        {
            //j = i;
        }
        else
        {
            
                
                Detectors.at(i)->SubstractBaseline(baseline_region_end);
                Detectors.at(i)->FindFirstPeak(baseline_region_end, max_region_end);
                Detectors.at(i)->ConvertFirstPeak2GlobalMaximum();
                //Detectors.at(i)->FindGlobalMaximum(0, Detectors.at(i)->waveform_y.size()-1);

                Detectors.at(i)->FindStartPoint(baseline_region_end);
                Detectors.at(i)->FindEndPoint(max_region_end);
                //Detectors.at(i)->FindElectronPeakEndPoint();
                //Detectors.at(i)->CalculateCharges();
                //Detectors.at(i)->FindNaiveTiming();
                //Detectors.at(i)->FindRiseTime();
                //Detectors.at(i)->FindFirstPeak();
                //Detectors.at(i)->FindMaxDerivative();

                Detectors.at(i)->TimeTwentyPercent();
                //Detectors.at(i)->TimeInflection();
                //ref_time += Detectors.at(i)->Inflection.timing;
                ref_time = Detectors.at(i)->TwentyPercent.x;
                if (ref_time > 1.e3) ref_time = 15.;
                if(abs(Detectors.at(i)->global_maximum.y)>0.5e-3)normalization = 1. / Detectors.at(i)->global_maximum.y;
                //avers.at(i)->initial();
                avers.at(i)->SetWaveform(Detectors.at(i)->waveform_x, Detectors.at(i)->waveform_y, ref_time, normalization, baseline_region_end);
                avers.at(i)->StandardAverage();
            
        }
    }

    //ref_time/= n;
    //if (ref_time > 1.e3)
    //    ref_time = 15.;

    //double normalization = 1. / Detectors.at(j)->global_maximum.y;
    //for(int i = 0; i < Detectors.at(j)->waveform_y.size(); ++i)
    //{
    //    normalization += Detectors.at(j)->waveform_y.at(i);
    //}
    //normalization /= Detectors.at(j)->waveform_y.size();

    //aver.SetWaveform(Detectors.at(j)->waveform_x, Detectors.at(j)->waveform_y, ref_time, normalization, baseline_region_end);
}



void TestBeamSetup::Finalize_AverageTools(const char* outfile_name)
{
    //char str1[200];
    TFile *f = new TFile(outfile_name,"recreate");
    f->Close();
    delete f;

    for(int i =0; i<avers.size();++i)
    {
        //sprintf(str1, "CH%d%s.root", Channel_IDs.at(i),outfile_name);
        avers.at(i)->Finalize();
        avers.at(i)->Write(outfile_name,Channel_IDs.at(i));
    }
}
void TestBeamSetup::init(std::vector<int> channel_ids)
{
    Channel_IDs = channel_ids;
    NofDetectors = Detectors.size();
    //max_region_end = 20001;
    init_tree();
}

void TestBeamSetup::tree_add_tracks(int &NumberofTracks, Track &OneTrack)
{
    OutTree->Branch("NumberofTracks", &NumberofTracks, "NumberofTracks/I");
    OutTree->Branch("MCP1_track_hit", &OneTrack.Hit1);
    OutTree->Branch("MM2_track_hit", &OneTrack.Hit2);
    OutTree->Branch("MM1_track_hit", &OneTrack.Hit3);
    OutTree->Branch("TrackChi2", &OneTrack.Chi2, "TrackChi2/D");
    OutTree->Branch("SlopeXZ", &OneTrack.SlopeXZ, "SlopeXZ/D");
    OutTree->Branch("SlopeYZ", &OneTrack.SlopeYZ, "SlopeYZ/D");
}

void TestBeamSetup::init(std::vector<int> channel_ids, int &NumberofTracks, Track &OneTrack)
{
    init(channel_ids);
    tree_add_tracks(NumberofTracks, OneTrack);
}

void TestBeamSetup::init_tree()
{
    //OutFile = new TFile("outrootfile.root","recreate");
    OutTree = new TTree("Pico", "Analysis Output");
    int tr_no = 0;
    int mcp_no = 0;
    int mm_no = 0;
    int filter = 0;
    std::cout << "===List of different types of characteristics===" << std::endl;
    OutTree->Branch("Baseline_Window", &baseline_region_end, "Baseline_Window/I");
    for (int i = 0; i < Detectors.size(); ++i)
    {
        Detector *det;
        int type = Detectors.at(i)->type;
        std::string typestr;
        if (type > 0)
        {

            mm_no++;
            char str[20];
            sprintf(str, "MM%d_", mm_no);
            typestr = str;
        }

        if (type == 0)
        {
            tr_no = Channel_IDs.at(i);
            char str[20];
            sprintf(str, "TR%d_", tr_no);
            typestr = str;
        }
        if (type < 0)
        {
            mcp_no = Channel_IDs.at(i);
            char str[20];
            sprintf(str, "MCP%d_", mcp_no);
            typestr = str;
        }
        bool is_filter = 0;
        if (type > 0)
        {
            if (filter == mm_no - 1)
            {
                det = Detectors.at(i);
                //std::cout << "fmm" << i << std::endl;
                filter++;
                i--;
                mm_no--;
                is_filter = 1;
                typestr = "Filtered_" + typestr;
            }
            else
            {
                det = Detectors.at(i)->pre_filter_backup;
                //std::cout << "mm" << i << std::endl;
            }
        }
        else
        {
            det = Detectors.at(i);
            //std::cout << "mcp" << i << std::endl;
        }

        std::string varname;
        std::string leafname;

        varname = typestr + "baseline_level";
        leafname = varname + "/D";
        if (!is_filter)
            OutTree->Branch(varname.c_str(), &det->baseline_level, leafname.c_str());

        varname = typestr + "baseline_rms";
        leafname = varname + "/D";
        if (!is_filter)
            OutTree->Branch(varname.c_str(), &det->baseline_rms, leafname.c_str());

        varname = typestr + "global_maximum_y";
        leafname = varname + "/D";
        OutTree->Branch(varname.c_str(), &det->global_maximum.y, leafname.c_str());

        varname = typestr + "global_maximum_x";
        leafname = varname + "/D";
        OutTree->Branch(varname.c_str(), &det->global_maximum.x, leafname.c_str());

        varname = typestr + "start_x";
        leafname = varname + "/D";
        OutTree->Branch(varname.c_str(), &det->start_point.x, leafname.c_str());
        varname = typestr + "end_x";
        leafname = varname + "/D";
        OutTree->Branch(varname.c_str(), &det->end_point.x, leafname.c_str());
        varname = typestr + "invert_maximum_x";
        leafname = varname + "/D";
        OutTree->Branch(varname.c_str(), &det->invert_maximum.x, leafname.c_str());
        varname = typestr + "invert_maximum_y";
        leafname = varname + "/D";
        OutTree->Branch(varname.c_str(), &det->invert_maximum.y, leafname.c_str());

        varname = typestr + "secondinvertpeak_x";
        leafname = varname + "/D";
        OutTree->Branch(varname.c_str(), &det->SecondInvertPeak.x, leafname.c_str());

        varname = typestr + "secondinvertpeak_y";
        leafname = varname + "/D";
        OutTree->Branch(varname.c_str(), &det->SecondInvertPeak.y, leafname.c_str());

        /*
           varname = typestr + "e_peak_end_x";
           leafname = varname + "/D";
           OutTree->Branch(varname.c_str(),&det->e_peak_end.x,leafname.c_str());

           varname = typestr + "inflection_timing";
           leafname = varname + "/D";
           if(is_filter || type < 0)
           OutTree->Branch(varname.c_str(),&det->Inflection.timing,leafname.c_str());

           varname = typestr + "inflection_failed";
           leafname = varname + "/O";
           if(is_filter || type < 0)
           OutTree->Branch(varname.c_str(),&det->Inflection.failed,leafname.c_str());

           varname = typestr + "half_charge";
           leafname = varname + "/D";
           OutTree->Branch(varname.c_str(),&det->charge_leading_edge,leafname.c_str());

           varname = typestr + "e_charge";
           leafname = varname + "/D";
           OutTree->Branch(varname.c_str(),&det->charge_e_peak,leafname.c_str());
           */
        varname = typestr + "all_charge";
        leafname = varname + "[4]/D";
        OutTree->Branch(varname.c_str(), det->charge_all, leafname.c_str());

        varname = typestr + "rise_time";
        leafname = varname + "[4]/D";
        OutTree->Branch(varname.c_str(), &det->rise_time, leafname.c_str());

        varname = typestr + "width";
        leafname = varname + "/D";
        OutTree->Branch(varname.c_str(), &det->width, leafname.c_str());

        varname = typestr + "CFDtime";
        leafname = varname + "[8]/D";
        if (is_filter || type <= 0)
            OutTree->Branch(varname.c_str(), det->CFDtime, leafname.c_str());
        varname = typestr + "CFDfrac";
        leafname = varname + "[8]/D";
        if (is_filter || type <= 0)
            OutTree->Branch(varname.c_str(), det->CFDfrac, leafname.c_str());
        varname = typestr + "CFDr";
        leafname = varname + "[8]/D";
        if (is_filter || type <= 0)
            OutTree->Branch(varname.c_str(), det->CFDr, leafname.c_str());
        varname = typestr + "CFDfailed";
        leafname = varname + "[8]/O";
        if (is_filter || type <= 0)
            OutTree->Branch(varname.c_str(), det->CFDfailed, leafname.c_str());

        /*
        varname = typestr + "LEDtime";
        leafname = varname + "[14]/D";
        if(is_filter || type < 0)
            OutTree->Branch(varname.c_str(),det->LEDtime,leafname.c_str());
        varname = typestr + "LEDthrd";
        leafname = varname + "[14]/D";
        if(is_filter || type < 0)
            OutTree->Branch(varname.c_str(),det->LEDthrd,leafname.c_str());
        varname = typestr + "LEDr";
        leafname = varname + "[14]/D";
        if(is_filter || type < 0)
            OutTree->Branch(varname.c_str(),det->LEDr,leafname.c_str());
        varname = typestr + "LEDfailed";
        leafname = varname + "[14]/O";
        if(is_filter || type < 0)
            OutTree->Branch(varname.c_str(),det->LEDfailed,leafname.c_str());
*/
        if (type > 0)
        {
            varname = typestr + "naive_time";
            leafname = varname + "/D";
            OutTree->Branch(varname.c_str(), &det->naive_point.x, leafname.c_str());

            varname = typestr + "sigmoid_parameters";
            leafname = varname + "[4]/D";
            OutTree->Branch(varname.c_str(), det->Sigmoid.parameters, leafname.c_str());

            varname = typestr + "sigmoid_chi_square";
            leafname = varname + "/D";
            OutTree->Branch(varname.c_str(), &det->Sigmoid.chisquare, leafname.c_str());

            varname = typestr + "sigmoid_failed";
            leafname = varname + "/O";
            OutTree->Branch(varname.c_str(), &det->Sigmoid.failed, leafname.c_str());

            varname = typestr + "first_peak_y";
            leafname = varname + "/D";
            OutTree->Branch(varname.c_str(), &det->first_peak.y, leafname.c_str());

            varname = typestr + "first_peak_x";
            leafname = varname + "/D";
            OutTree->Branch(varname.c_str(), &det->first_peak.x, leafname.c_str());
        }

        std::cout << typestr << std::endl;
    }
}
