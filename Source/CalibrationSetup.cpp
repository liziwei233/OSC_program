#include "CalibrationSetup.h"

void CalibrationSetup::CalibrationAnalysis()
{
    ScaleAndShiftTimes();
    int i = 0;
    Detectors.at(i)->InvertY();
    //Dynamically find the end for the baseline calculation region
    Detectors.at(i)->FindGlobalMaximum(0, Detectors.at(i)->waveform_y.size()-1);
    baseline_region_end = Detectors.at(i)->global_maximum.position-200;
    if(baseline_region_end < 2) baseline_region_end = 10;
    //===

    Detectors.at(i)->SubstractBaseline(baseline_region_end);

    Detectors.at(i)->FindGlobalMaximum(baseline_region_end, Detectors.at(i)->waveform_y.size()-1); 
    Detectors.at(i)->FindStartPoint(baseline_region_end);
    Detectors.at(i)->FindEndPoint(max_region_end);
    Detectors.at(i)->FindElectronPeakEndPoint();
    Detectors.at(i)->CalculateCharges();
    Detectors.at(i)->FindNaiveTiming();
    Detectors.at(i)->FindRiseTime();
    Detectors.at(i)->FindFirstPeak();
    Detectors.at(i)->FindMaxDerivative();

    Detectors.at(i)->TimeSigmoid();
    Detectors.at(i)->FilterWaveformFFT(baseline_region_end, 4096, 1.5);
    Detectors.at(i)->FindGlobalMaximum(0, 4095); 
    Detectors.at(i)->FindStartPoint(0);
    Detectors.at(i)->FindEndPoint(max_region_end);
    Detectors.at(i)->FindElectronPeakEndPoint();
    Detectors.at(i)->CalculateCharges();
    Detectors.at(i)->FindNaiveTiming();
    Detectors.at(i)->FindRiseTime();
    Detectors.at(i)->FindFirstPeak();
    Detectors.at(i)->FindMaxDerivative();
    Detectors.at(i)->TimeSigmoid();
    Detectors.at(i)->TimeInflection();
    Detectors.at(i)->TimeTwentyPercent();
}

void CalibrationSetup::SetWaveformToAverage(AverageTool &aver)
{
    baseline_region_end=2000000;
    for(int i = 0; i < NofDetectors; ++i)
    {
        Detectors.at(i)->FindGlobalMaximum(0, Detectors.at(i)->waveform_y.size()-1);
        if(Detectors.at(i)->global_maximum.position < baseline_region_end)
            baseline_region_end = Detectors.at(i)->global_maximum.position;
    }
    baseline_region_end-=200;
    if(baseline_region_end < 2) baseline_region_end = 2;
    //===
    //
    int j = 0;
    double ref_time=0;
    double n = 0;
    int i = 0;
    ScaleAndShiftTimes();
    Detectors.at(i)->InvertY();
    //Dynamically find the end for the baseline calculation region
    Detectors.at(i)->FindGlobalMaximum(0, Detectors.at(i)->waveform_y.size()-1);
    baseline_region_end = Detectors.at(i)->global_maximum.position-200;
    if(baseline_region_end < 2) baseline_region_end = 10;
    //===
    
    Detectors.at(i)->SubstractBaseline(baseline_region_end);

    Detectors.at(i)->FindGlobalMaximum(baseline_region_end, max_region_end); 
    Detectors.at(i)->FindStartPoint(baseline_region_end);
    Detectors.at(i)->FindEndPoint(max_region_end);
    Detectors.at(i)->FindElectronPeakEndPoint();
    Detectors.at(i)->CalculateCharges();
    Detectors.at(i)->FindNaiveTiming();
    Detectors.at(i)->FindRiseTime();
    Detectors.at(i)->FindFirstPeak();
    Detectors.at(i)->FindMaxDerivative();

    Detectors.at(i)->TimeSigmoid();
    double *p = Detectors.at(i)->Sigmoid.parameters;
    ref_time = p[1] - (1./p[2])*TMath::Log(p[0]/(0.2*Detectors.at(i)->global_maximum.y-p[3]) -1);
    //ref_time += Detectors.at(i)->Inflection.timing;
    //ref_time += Detectors.at(i)->TwentyPercent.x;

    double normalization = 1./Detectors.at(j)->global_maximum.y;
    double ph = Detectors.at(j)->global_maximum.y;
    //if(ph < 0.03 && ph > 0.06) baseline_region_end = -1;
    aver.SetWaveform(Detectors.at(j)->waveform_x, Detectors.at(j)->waveform_y, ref_time, normalization, baseline_region_end);
}

void CalibrationSetup::Dump()
{
    TFile dumpfile("calibration_dumpfile.root","recreate");
    //TTree *newtree = OutTree->CloneTree(0);
    //newtree->Fill();

    TObject integer;

    integer.SetUniqueID(Detectors.size());
    integer.Write("n");

    TGraph gr;
    for(int i = 0; i < Detectors.size() ; ++i)
    {
        gr = TGraph(Detectors.at(i)->waveform_x.size(), &Detectors.at(i)->waveform_x[0], &Detectors.at(i)->waveform_y[0]);
        char str1[20];
        sprintf(str1,"graph_%d",i);
        gr.Write(str1);
        if( Detectors.at(i)->pre_filter_backup ) 
        {
            gr = TGraph(Detectors.at(i)->pre_filter_backup->waveform_x.size(), &Detectors.at(i)->pre_filter_backup->waveform_x[0], &Detectors.at(i)->pre_filter_backup->waveform_y[0]);
            sprintf(str1,"unfiltered_graph_%d",i);
            gr.Write(str1);
        }
    }
    //Detectors.at(0)->pre_filter_backup->Sigmoid.fit_func.Write("rawsigmoid");
    //newtree->Write();
    dumpfile.Close();

}

void CalibrationSetup::init()
{
    NofDetectors=Detectors.size();
    max_region_end = 3001;
    init_tree();
}

void CalibrationSetup::init_tree()
{
    //OutFile = new TFile("outrootfile.root","recreate");
    OutTree = new TTree("Pico","Analysis Output");
    int mcp_no = 0;
    int mm_no = 0;
    int filter = 0;
    std::cout << "===List of different types of characteristics===" << std::endl;
    OutTree->Branch("Baseline_Window",&baseline_region_end,"Baseline_Window/I");
    for(int i = 0; i < Detectors.size(); ++i)
    {
        Detector* det;
        int type = Detectors.at(i)->type;
        std::string typestr;
        if(type > 0)
        {
            mm_no++;
            char str[20];
            sprintf(str,"MM%d_",mm_no);
            typestr = str;
        }
        else
        {
            mcp_no++;
            char str[20];
            sprintf(str,"PD%d_",mcp_no);
            typestr = str;
        }
        bool is_filter = 0; 
        if(type > 0)
        {
            if(filter == mm_no - 1)
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
        if(!is_filter)
            OutTree->Branch(varname.c_str(),&det->baseline_level,leafname.c_str());

        varname = typestr + "baseline_rms";
        leafname = varname + "/D";
        if(!is_filter)
            OutTree->Branch(varname.c_str(),&det->baseline_rms,leafname.c_str());

        varname = typestr + "global_maximum_y";
        leafname = varname + "/D";
        OutTree->Branch(varname.c_str(),&det->global_maximum.y,leafname.c_str());

        varname = typestr + "global_maximum_x";
        leafname = varname + "/D";
        OutTree->Branch(varname.c_str(),&det->global_maximum.x,leafname.c_str());

        varname = typestr + "start_x";
        leafname = varname + "/D";
        OutTree->Branch(varname.c_str(),&det->start_point.x,leafname.c_str());

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

        varname = typestr + "all_charge";
        leafname = varname + "/D";
        OutTree->Branch(varname.c_str(),&det->charge_all,leafname.c_str());

        varname = typestr + "rise_time";
        leafname = varname + "/D";
        OutTree->Branch(varname.c_str(),&det->rise_time,leafname.c_str());

        varname = typestr + "twentypercent_time";
        leafname = varname + "/D";
        if(is_filter || type < 0)
            OutTree->Branch(varname.c_str(),&det->TwentyPercent.x,leafname.c_str());

        varname = typestr + "twentypercent_failed";
        leafname = varname + "/O";
        if(is_filter || type < 0)
            OutTree->Branch(varname.c_str(),&det->TwentyPercent.failed,leafname.c_str());

        if(type > 0)
        {
            varname = typestr + "naive_time";
            leafname = varname + "/D";
            OutTree->Branch(varname.c_str(),&det->naive_point.x,leafname.c_str());

            varname = typestr + "sigmoid_parameters";
            leafname = varname + "[4]/D";
            OutTree->Branch(varname.c_str(),det->Sigmoid.parameters,leafname.c_str());

            varname = typestr + "sigmoid_chi_square";
            leafname = varname + "/D";
            OutTree->Branch(varname.c_str(),&det->Sigmoid.chisquare,leafname.c_str());

            varname = typestr + "sigmoid_failed";
            leafname = varname + "/O";
            OutTree->Branch(varname.c_str(),&det->Sigmoid.failed,leafname.c_str());

            varname = typestr + "first_peak_y";
            leafname = varname + "/D";
            OutTree->Branch(varname.c_str(),&det->first_peak.y,leafname.c_str());

            varname = typestr + "first_peak_x";
            leafname = varname + "/D";
            OutTree->Branch(varname.c_str(),&det->first_peak.x,leafname.c_str());
        }

        std::cout << typestr << std::endl;
    }
}

void CalibrationSetup::SetCividecWaveformToAverage(AverageTool &aver)
{
    aver.SetWaveform(Detectors.at(0)->waveform_x, Detectors.at(0)->waveform_y, 0., 1., 40.);
}

