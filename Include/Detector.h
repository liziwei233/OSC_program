#ifndef __detector_h__
#define __detector_h__

#include <vector>

#include <TMath.h>
#include <TVirtualFFT.h>
#include <TMatrixD.h>
#include <TGraphErrors.h>
#include <TF1.h>

#include <DetectorSetup.h>

struct WaveformPoint
{
    double y;
    double x;
    int position;
};

struct TimingInfo
{
    double y;
    double x;
    double slope;
    double intersect;
    double timing;
    double parameters[4];//pol3 parameters
    bool failed = 0;
};



struct SigmoidInfo
{
    TF1 fit_func;
    double parameters[4];
    bool failed;
    double chisquare;
    int degrees_freedom;
};

class Detector
{
    friend class DetectorSetup;
    friend class MuonSetup;
    friend class LaserSetup;
    friend class CalibrationSetup;
    friend class SimulationSetup;
    friend class TestBeamSetup;

    public:
    Detector();
    ~Detector();
    void SetWaveY(std::vector<double> wave);
    void SetWaveX(std::vector<double> wave);
    void SetMCP();
    void SetMM();
    void InvertY();
    void SubstractBaseline(int base_region_end);
    void FindGlobalMaximum(int start, int end);
    void FindStartPoint(int start);
    void FindEndPoint(int start);
    void FindElectronPeakEndPoint();
    void FindElectronPeakEndPointSimulation();
    void CalculateCharges();
    void FindNaiveTiming();
    double linear_interpolation(double x1, double x2, double y1, double y2, double y);
    void FindFirstPeak();
    void FindMaxDerivative();
    void TimeInflection();
    bool FitPol3(double* x, double* y, double* fit_parameters);
    void TimeTwentyPercent();
    void TimeInformation();
    TimingInfo Time(double fac,int partype);
    WaveformPoint FindTimingPoint(double fac,int partype);
    void TimeSigmoid();
    void FindRiseTime();
    void FindWidth();
    //void FilterWaveformFFT_test();
    void FilterWaveformFFT(int start, int N, double biggest_frequency_not_to_cut_GHz);
    private:
    int type;

    std::vector<double> waveform_y;
    std::vector<double> waveform_x;

    Detector* pre_filter_backup;

    double baseline_level;
    double baseline_rms;

    WaveformPoint global_maximum;
    WaveformPoint start_point;
    WaveformPoint end_point;
    WaveformPoint e_peak_end;
    WaveformPoint naive_point;
    WaveformPoint first_peak;
    WaveformPoint max_derivative;

    double charge_leading_edge;
    double charge_e_peak;
    double charge_all[4];

    double naive_time;//ns
    double rise_time;// defined from 20% to 80% height of the pulse
    double width;

    TimingInfo Inflection;
    TimingInfo TwentyPercent;
    SigmoidInfo Sigmoid;
    
    double CFDtime[8];
    double CFDfrac[8];
    bool CFDfailed[8];

    double LEDtime[14];
    double LEDthrd[14];
    bool LEDfailed[14];

    TimingInfo CFD;
    TimingInfo LED;
    TimingInfo Lead_percent_5;
    TimingInfo Lead_percent_10;
    TimingInfo Lead_percent_15;
    TimingInfo Lead_percent_20;
    TimingInfo Lead_percent_25;
    TimingInfo Lead_percent_30;
    TimingInfo Lead_percent_35;
    TimingInfo Lead_percent_40;

    TimingInfo Lead_thrd_30;
    TimingInfo Lead_thrd_50;
    TimingInfo Lead_thrd_70;
    TimingInfo Lead_thrd_90;
    TimingInfo Lead_thrd_110;
    TimingInfo Lead_thrd_130;
    TimingInfo Lead_thrd_150;
    TimingInfo Lead_thrd_170;
    TimingInfo Lead_thrd_190;
    TimingInfo Lead_thrd_210;
    TimingInfo Lead_thrd_230;
    TimingInfo Lead_thrd_250;
    TimingInfo Lead_thrd_300;
    TimingInfo Lead_thrd_400;
};

double fermi_dirac(double *x, double *par);

#endif
