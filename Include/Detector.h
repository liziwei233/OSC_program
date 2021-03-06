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
    double r;
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
    void SetTR();
    void SetMM();
    void InvertY();
    void SubstractBaseline(int base_region_end);
    void FindGlobalMaximum(int start, int end);
    void FindFirstPeak(int start, int end);
    void ConvertFirstPeak2GlobalMaximum();
    void FindInvertMaximum(int start, int end);
    void FindSecondInvertPeak(int start);
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
    void LineFitLeastSquares(double *data_x, double *data_y, int data_n, std::vector<double> &vResult);
    void TimeTwentyPercent();
    void TimeInformation(int Dtype, int fittype, int npoint);
    TimingInfo Time(double fac,int partype);
    TimingInfo Time_linear(double fac,int partype,int Npoint);
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
    WaveformPoint firstpeak;
    WaveformPoint invert_maximum;
    WaveformPoint SecondInvertPeak;
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
    double rise_time[4];// 1,2 defined from 10-90% height of the pulse; 3,4 defined from 20-80%.
    double width;

    TimingInfo Inflection;
    TimingInfo TwentyPercent;
    SigmoidInfo Sigmoid;
    
    TimingInfo CFD;
    double CFDtime[8];
    double CFDfrac[8];
    bool CFDfailed[8];
    double CFDr[8];

/*
    double LEDtime[14];
    double LEDthrd[14];
    bool LEDfailed[14];
    double LEDr[14];
    TimingInfo LED;
*/

};

double fermi_dirac(double *x, double *par);

#endif
