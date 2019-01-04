#include "Detector.h"

Detector::Detector()
{
    pre_filter_backup = 0;
}

Detector::~Detector()
{
    if(pre_filter_backup)
        delete pre_filter_backup;
}

void Detector::SetWaveY(std::vector<double> wave)
{
    waveform_y = wave;
}

void Detector::SetWaveX(std::vector<double> wave)
{
    waveform_x = wave;
}

void Detector::SetMCP()
{
    type = -1;
}

void Detector::SetMM()
{
    type = 1;
    pre_filter_backup = new Detector;
}

void Detector::InvertY()
{
    //Inverts waveform_y
    std::transform(waveform_y.begin(), waveform_y.end(), waveform_y.begin(),[](double y){ return -y; });
}

void Detector::SubstractBaseline(int base_region_end)
{
    //Find baseline
    double baseline_sum=0;
    double baseline_square_sum=0;
    int base_region_start = base_region_end - 2000;
    if (base_region_start < 0 ) base_region_start = 40;
    if (base_region_start > base_region_end) base_region_start = 0;
    for(int i = base_region_start; i < base_region_end; ++i)
    {
        double y = waveform_y.at(i);
        baseline_sum+=y;
        baseline_square_sum+=y*y;
    }
    baseline_level = baseline_sum/(base_region_end-base_region_start);
    double baseline_rms_squared = baseline_square_sum/(base_region_end-base_region_start) - baseline_level*baseline_level ;
    baseline_rms = baseline_rms_squared > 0 ? TMath::Sqrt(baseline_rms_squared) : 0;

    //Subtract baseline
    std::transform(waveform_y.begin(), waveform_y.end(), waveform_y.begin(),
                      bind2nd(std::plus<double>(), -baseline_level));
}

void Detector::FindGlobalMaximum(int start, int end)
{
    global_maximum.y=-111111;
    global_maximum.position=5;

    for(int i = start ; i <= end && i < waveform_y.size() ; i++)
    { 
        if( waveform_y.at(i) > global_maximum.y )
        {
            global_maximum.y = waveform_y.at(i);
            global_maximum.position = i;
        }
    }
    global_maximum.x = waveform_x.at(global_maximum.position);
}

void Detector::FindStartPoint(int start)
{
    start_point.x = 0;
    start_point.y = 0;
    start_point.position = 1;
    for(int i = global_maximum.position; i >= start; --i)
    {
        if(waveform_y.at(i) - baseline_rms < 0 )
        {
            start_point.y = waveform_y.at(i);
            start_point.position = i;
            start_point.x = waveform_x.at(i);
            break;
        }
    }
}

void Detector::FindEndPoint(int start)
{
    int N = waveform_y.size();

    end_point.x = 0;
    end_point.y = 0;
    end_point.position = N-1;

    int j = type > 0 ? start : global_maximum.position;
    for(int i = j ; i < N; ++i)
    {
        if( waveform_y.at(i) - baseline_rms < 0 )
        {
            end_point.y = waveform_y.at(i);
            end_point.position = i;
            end_point.x = waveform_x.at(i);
            break;
        }
    }
}

void Detector::FindElectronPeakEndPoint()
{
    if(type < 0) // is MCP
    {
        e_peak_end = end_point;
    }
    else  //is MM
    {
        e_peak_end.y = 11111111111.;
        e_peak_end.position = global_maximum.position;

        int j = global_maximum.position + 60;
        if( j > waveform_y.size()-1 ) j = waveform_y.size()-2;

        for(int i = global_maximum.position + 3; i < j && i+1 < waveform_y.size() ; ++i)
        {
            if( waveform_y.at(i) < e_peak_end.y )
            {
                e_peak_end.y = waveform_y.at(i);
                e_peak_end.position = i;
                if(waveform_y.at(i+1) < 0) 
                    break;
            }
        }
        e_peak_end.x = waveform_x.at(e_peak_end.position);
    }
}

void Detector::FindElectronPeakEndPointSimulation()
{
    if(type < 0) // is MCP
    {
        e_peak_end = end_point;
    }
    else  //is MM
    {
        int n = 15;
        int j = global_maximum.position + 60;
        if( j >= waveform_y.size()-n ) j = waveform_y.size()-1-n;

        e_peak_end.position = j;
        e_peak_end.y = waveform_y.at(j);
        

        for(int i = global_maximum.position + 3; i < j; ++i)
        {
            double mean_x = 0, mean_y = 0;
            for(int k = 0; k < n; k++)
            {
                mean_y += waveform_y.at(i+k);
                mean_x += waveform_x.at(i+k);
            }
            mean_y /= n;
            mean_x /= n;

            double num = 0, den = 0;
            for(int k = 0; k < n; k++)
            {
                num += (waveform_x.at(i+k) - mean_x)*(waveform_y.at(i+k) - mean_y);
                den += (waveform_x.at(i+k) - mean_x)*(waveform_x.at(i+k) - mean_x);
            }

            double slope = num/den;
            if ( abs(slope) < 0.001 ) 
            {
                e_peak_end.position = i;
                e_peak_end.y = waveform_y.at(i);
                break;
            }
        }
        e_peak_end.x = waveform_x.at(e_peak_end.position);
    }
}

void Detector::CalculateCharges()
{
    double Ohms = 50;
    double step = waveform_x.at(1)- waveform_x.at(0);
    double conversion = step/Ohms*1000;

    charge_leading_edge=0;
    for(int i = start_point.position; i <= global_maximum.position; ++i)
        charge_leading_edge += waveform_y.at(i);
    charge_leading_edge *= conversion;

    charge_e_peak=0;
    for(int i = start_point.position; i <= e_peak_end.position; ++i)
        charge_e_peak += waveform_y.at(i);
    charge_e_peak *= conversion;

    charge_all=0;
    for(int i = start_point.position; i <= end_point.position; ++i)
        charge_all += waveform_y.at(i);
    charge_all *= conversion;
}

void Detector::FindNaiveTiming()
{
    double cf = 0.2*global_maximum.y;
    for(int i = global_maximum.position ; i > start_point.position && i>1; --i)
    {
        if( waveform_y.at(i) - cf > 0 && waveform_y.at(i-1) - cf < 0 )
        {
            double x1 = waveform_x.at(i-1);
            double x2 = waveform_x.at(i);
            double y1 = waveform_y.at(i-1);
            double y2 = waveform_y.at(i);
            naive_time = linear_interpolation(x1,x2,y1,y2,cf);
            naive_point.y = waveform_y.at(i);
            naive_point.position = i;
            naive_point.x = waveform_x.at(i);
            break;
        }
    }
}

double Detector::linear_interpolation(double x1, double x2, double y1, double y2, double y)
{
    double b = (y2-y1)/(x2-x1);
    double a = y2 - b*x2;
    double x = (y-a)/b;
    return x;
}

void Detector::FindFirstPeak()
{
    first_peak = global_maximum;
    int j = global_maximum.position;
    while(j+3 > waveform_y.size()) j--;
    if(type > 0) //is MM
    {
        for(int i = naive_point.position; i <= j && i+3 < waveform_y.size(); ++i)
        {
            while( i < 3 ) i++;

            double dm2 = waveform_y.at(i-2) - waveform_y.at(i-3); 
            double dm1 = waveform_y.at(i-1) - waveform_y.at(i-2); 
            double dm = waveform_y.at(i) - waveform_y.at(i-1); 
            double dp = waveform_y.at(i+1) - waveform_y.at(i); 
            double dp1 = waveform_y.at(i+2) - waveform_y.at(i+1); 
            double dp2 = waveform_y.at(i+3) - waveform_y.at(i+2);
            if(dm2 > 0 && dm1 >= 0 && dm >= 0 && dp <= 0 && dp1 <= 0 && dp2 < 0 && waveform_y.at(i) >= 0.4*global_maximum.y)
            {
                first_peak.y = waveform_y.at(i);
                first_peak.position = i;
                first_peak.x = waveform_x.at(i);
                break;
            }
        }
    }
}

void Detector::FindMaxDerivative()
{
    max_derivative.y = -1111111111;
    max_derivative.position = first_peak.position;

    for(int i = naive_point.position - 2; i < first_peak.position - 2 && i+1 < waveform_y.size(); ++i)
    {
        while (i < 0 ) i++;

        double dy = waveform_y.at(i+1) - waveform_y.at(i); 
        if( dy > max_derivative.y )
        {
            max_derivative.y = dy;
            max_derivative.position = i;
        }
    }
    max_derivative.x = waveform_x.at(max_derivative.position);
}

void Detector::TimeInflection()
{
    bool use_filtered_waveform = 0;
    int k = max_derivative.position;
    while(k-2 < 0) k++;
    while( k+2 >= waveform_y.size() ) k--;
    
    double y[4],x[4];
    double x_offset = waveform_x.at(k-2); //scale for numerical purposes
    for(int i = 0 ; i < 4; ++i)
    {
        x[i] = waveform_x.at(k-1+i) - x_offset;
        y[i] = waveform_y.at(k-1+i);
    }
    double par[4];
    bool success = FitPol3(x,y,par);
    if(!success)
    {
        Inflection.failed = 1;
        return;
    }

    Inflection.y = par[0] - par[1]*par[2]/3./par[3] + 2*par[2]*par[2]*par[2]/27./par[3]/par[3];
    Inflection.slope = par[1] - par[2]*par[2]/3./par[3];
    Inflection.x = -par[2]/3./par[3] + x_offset;
    Inflection.intersect = Inflection.y - Inflection.slope*Inflection.x;
    Inflection.timing = -Inflection.intersect/Inflection.slope;
    for(int i = 0; i < 4; ++i) Inflection.parameters[i] = par[i];
}

bool Detector::FitPol3(double* x, double* y, double* fit_parameters)
{
    TMatrixD X(4,4);
    for(int i = 0 ; i < 4; ++i)
    {
        X[i][0] = 1.;        
        X[i][1] = x[i];        
        X[i][2] = x[i]*x[i];        
        X[i][3] = x[i]*x[i]*x[i];        
    }
    TVectorD Y(4,y);
    double det;
    TVectorD pars = X.Invert(&det)*Y;

    if(det == 0) return 0;

    for(int i = 0; i < 4; ++i)
        fit_parameters[i] = pars[i];
    
    return 1;
}

void Detector::TimeTwentyPercent()
{
    TwentyPercent.failed = 0;
    //if(type > 0) return;
    bool use_filtered_waveform = 0;
    int k = naive_point.position;
    while(k-3 < 0) k++;
    while( k+3 >= waveform_y.size() ) k--;
    
    double y[4],x[4];
    double x_offset = waveform_x.at(k-2); //scale for numerical purposes
    for(int i = 0 ; i < 4; ++i)
    {
        x[i] = waveform_x.at(k-1+i) - x_offset;
        y[i] = waveform_y.at(k-1+i);
    }
    double par[4];
    bool success = FitPol3(x,y,par);
    if(!success)
    {
        TwentyPercent.failed = 1;
        return;
    }
    
    double par_inverse[4];
    success = FitPol3(y,x,par_inverse);
    if(!success)
    {
        TwentyPercent.failed = 1;
        return;
    }

    double x20 = par_inverse[0];
    double y20 = 0.2*global_maximum.y;
    double y20_pow = 1;
    for(int i = 1 ; i < 4; ++i)
    {
        y20_pow*=y20; 
        x20+=par_inverse[i]*y20_pow;
    }
    double y20e = par[0];
    double x20_pow = 1;
    for(int i = 1; i < 4; ++i)
    {
        x20_pow *= x20;
        y20e += par[i]*x20_pow; 
    }

    TwentyPercent.x = x20 + x_offset;
    TwentyPercent.y = y20e;
    TwentyPercent.slope = par[1] + 2*par[2]*x20 + 3*par[3]*x20*x20;
    TwentyPercent.intersect = TwentyPercent.y - TwentyPercent.slope*TwentyPercent.x;
    TwentyPercent.timing = -TwentyPercent.intersect/TwentyPercent.slope;
    for(int i = 0; i < 4; ++i) TwentyPercent.parameters[i] = par[i];

}

void Detector::TimeSigmoid()
{
    int start = start_point.position - 1; 
    while(start < 0) start++;
    int end = first_peak.position + 5;
    while(end >= waveform_y.size()) end--;
    int Npoints = end-start+1;
    if( Npoints > 100 || Npoints <=0 )
    {
        Sigmoid.failed = 1;
        return;
    }

    double x[Npoints], y[Npoints], erx[Npoints], ery[Npoints];
    for(int i = start; i <= end; ++i)
    {
        x[i-start] = waveform_x.at(i);
        y[i-start] = waveform_y.at(i);
        erx[i-start] = 0;
        ery[i-start] = baseline_rms;
    }

    TGraphErrors waveform_graph(Npoints, x, y, erx, ery);

    double pars[4];
    pars[0]=first_peak.y;
    pars[1]=(x[end-start] + x[1])/2;
    pars[2]=5./(x[end-start] - x[1]);
    pars[3]=0.;
    
    TF1 fd_fit("fd_fit", fermi_dirac, x[0], x[Npoints-1], 4);
    fd_fit.SetParameters(pars[0],pars[1],pars[2],pars[3]);
    waveform_graph.Fit("fd_fit","qR");
    waveform_graph.Fit("fd_fit","qR");
    waveform_graph.Fit("fd_fit","qR");
    waveform_graph.Fit("fd_fit","qR");

    Sigmoid.chisquare = fd_fit.GetChisquare();
    Sigmoid.parameters[0] = fd_fit.GetParameter(0);
    Sigmoid.parameters[1] = fd_fit.GetParameter(1);
    Sigmoid.parameters[2] = fd_fit.GetParameter(2);
    Sigmoid.parameters[3] = fd_fit.GetParameter(3);
    Sigmoid.degrees_freedom = fd_fit.GetNDF();
    Sigmoid.chisquare = fd_fit.GetChisquare()/fd_fit.GetNDF();
    Sigmoid.fit_func = fd_fit;
    Sigmoid.failed = Sigmoid.chisquare < 1000 ? 0 : 1;
}

double fermi_dirac(double *x, double *par)
{
    double fdreturn = par[0]/(1+TMath::Exp(-(x[0]-par[1])*par[2]))+par[3];
    return fdreturn;
}


void Detector::FilterWaveformFFT(int start, int N, double biggest_frequency_not_to_cut_GHz)
{
    *pre_filter_backup = *this;
    pre_filter_backup->pre_filter_backup = 0;
    
    double temp_wave[N];
    double temp_x[N];
    int end = start + N;
    double delta = waveform_x.at(1) -  waveform_x.at(0);
    int waveform_size = waveform_y.size();
    for(int i = start ; i < end; ++i)
        temp_wave[i-start] = waveform_y.at(i%waveform_size);


    TVirtualFFT *fft_forward = TVirtualFFT::FFT(1, &N, "R2C M");
    fft_forward->SetPoints(temp_wave);
    fft_forward->Transform();

    
    double fourier_real[N];
    double fourier_imag[N];
    fft_forward->GetPointsComplex(fourier_real, fourier_imag);

    for(int i = 0; i < N; ++i)
    {   
        double frequency = i/delta/N;

        if(frequency > biggest_frequency_not_to_cut_GHz)// Cut at this frequency
        {
            fourier_real[i] = 0;
            fourier_imag[i] = 0;
        }
        //double filter = 1.;
        //if(frequency > 0.8)
        //    filter = TMath::Exp(2.08-2.6*frequency);
        //fourier_real[i] *= filter;
        //fourier_imag[i] *= filter;
    }

    TVirtualFFT *fft_backward = TVirtualFFT::FFT(1, &N, "C2R M");
    fft_backward->SetPointsComplex(fourier_real, fourier_imag);
    fft_backward->Transform();

    std::vector<double> temp_waveform_y;
    std::vector<double> temp_waveform_x;
    for(int i = 0; i < N; ++i)
    {
        temp_waveform_y.push_back(fft_backward->GetPointReal(i)/N);
        temp_waveform_x.push_back(waveform_x.at(start)+i*delta);
    }
    waveform_y = temp_waveform_y;
    waveform_x = temp_waveform_x;
}

void Detector::FindRiseTime()
{
    double eightypercent_naive_time;
    double cf = 0.8*global_maximum.y;
    for(int i = global_maximum.position ; i >= start_point.position && i > 1; --i)
    {
        if( waveform_y.at(i) - cf > 0 && waveform_y.at(i-1) - cf < 0 )
        {
            double x1 = waveform_x.at(i-1);
            double x2 = waveform_x.at(i);
            double y1 = waveform_y.at(i-1);
            double y2 = waveform_y.at(i);
            eightypercent_naive_time = linear_interpolation(x1,x2,y1,y2,cf);
            break;
        }
    }
    rise_time = eightypercent_naive_time - naive_time;
}

void Detector::FindWidth()
{
    double left_time;
    double right_time;
    double cf = 0.5*global_maximum.y;
    for(int i = global_maximum.position ; i >= start_point.position; --i)
    {
        if( waveform_y.at(i) - cf > 0 && waveform_y.at(i-1) - cf < 0 )
        {
            double x1 = waveform_x.at(i-1);
            double x2 = waveform_x.at(i);
            double y1 = waveform_y.at(i-1);
            double y2 = waveform_y.at(i);
            left_time = linear_interpolation(x1,x2,y1,y2,cf);
            break;
        }
    }

    bool ion_tail_is_too_high = 1;
    for(int i = global_maximum.position ; i <= e_peak_end.position; ++i)
    {
        if( waveform_y.at(i) - cf > 0 && waveform_y.at(i+1) - cf < 0 )
        {
            double x1 = waveform_x.at(i);
            double x2 = waveform_x.at(i+1);
            double y1 = waveform_y.at(i);
            double y2 = waveform_y.at(i+1);
            right_time = linear_interpolation(x1,x2,y1,y2,cf);
            ion_tail_is_too_high = 0;
            break;
        }
    }
    if(!ion_tail_is_too_high)
    {
        width = right_time - left_time;
    }
    else
    {
        std::cout << "ERROR*** Ion tail is too high and width cannot be properly calculated" << std::endl;
        width  = e_peak_end.x - left_time;
    }
}


/* unsafe
void Detector::FilterWaveformFFT_test()
{
    *pre_filter_backup = *this;
    pre_filter_backup->pre_filter_backup = 0;
    
    int N = waveform_y.size();
    //round to next power of two
    N--;
    N |=  N >> 1;
    N |=  N >> 2;
    N |=  N >> 4;
    N |=  N >> 1;
    N |=  N >> 16;
    N++;
    //================
    double temp_wave[N];
    double temp_x[N];
    double delta = waveform_x.at(1) -  waveform_x.at(0);

    for(int i = 0 ; i < waveform_y.size(); ++i)
        temp_wave[i] = waveform_y.at(i);
    for(int i = waveform_y.size() ; i < N; ++i)
        temp_wave[i] = 0.;

    TVirtualFFT *fft_forward = TVirtualFFT::FFT(1, &N, "R2C M");
    fft_forward->SetPoints(temp_wave);
    fft_forward->Transform();
    
    double fourier_real[N];
    double fourier_imag[N];
    fft_forward->GetPointsComplex(fourier_real, fourier_imag);

    for(int i = 0; i < N; ++i)
    {   
        double frequency = i/delta/N;
        if(frequency > 1)//in GHz
        {
            fourier_real[i] = 0;
            fourier_imag[i] = 0;
        }
    }

    TVirtualFFT *fft_backward = TVirtualFFT::FFT(1, &N, "C2R M");
    fft_backward->SetPointsComplex(fourier_real, fourier_imag);
    fft_backward->Transform();

    waveform_y.clear();
    for(int i = 0; i < waveform_x.size(); ++i)
    {
        waveform_y.push_back(fft_backward->GetPointReal(i)/N);
    }
}
*/
