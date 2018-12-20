#include "Simulation_FileReader.h"

Simulation_FileReader::Simulation_FileReader(const char* file_name)
{
    input_file = fopen(file_name,"r");
}

bool Simulation_FileReader::GetNextEvent()
{
    //double time_step = 0.05;
    double time_step = 0.05;
    double scale_factor = 1.e-3;
    //double scale_factor = 1.;
    double y;
    std::vector<double> wave_x,wave_y;
    char comma=',';
    //for(int i = 0; i < length - 1; ++i)
    //{
    int i = 0;
    while( comma == ',')
    {
        if( fscanf(input_file, "%lf%c", &y, &comma) == EOF) 
            return 0;
        wave_y.push_back(y*scale_factor);
        wave_x.push_back(i*time_step);
        i++;
    }
    //if( fscanf(input_file, "%lf%c", &y, &comma) == EOF )
    //    return 0;
    //
    int length = i;
    int extended_length = 2048;
    //round to next power of two
    //extended_length--;
    //extended_length |=  extended_length >> 1;
    //extended_length |=  extended_length >> 2;
    //extended_length |=  extended_length >> 4;
    //extended_length |=  extended_length >> 1;
    //extended_length |=  extended_length >> 16;
    //extended_length++;
   
    wave_y.push_back(y*scale_factor);
    wave_x.push_back((length-1)*time_step);

    for(int i = length; i < extended_length; i++)
    {
        wave_y.push_back(0);
        wave_x.push_back(i*time_step);
    }
    
    Detectors->SetDetectorWaveform(0,wave_x,wave_y);
    return 1;
}

void Simulation_FileReader::SetDetectorSetup(DetectorSetup &det)
{
    Detectors = &det;
}
