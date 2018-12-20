#include "TRC_FileReader.h"

TRC_FileReader::TRC_FileReader(std::vector<int> channel_ids, const char* dir_path, int max_n_files, const char* file_midname, int first_file_counter)
{
    First_File = first_file_counter;
    for(int i = 0; i < channel_ids.size(); ++i)
    {
        TRC_channel C(dir_path, max_n_files, channel_ids.at(i), file_midname, First_File);
        Channels.push_back(C);
    }
}

void TRC_FileReader::OpenTriggerChannel(const char* dir_path, int max_n_files, const char* file_midname, int ch_id)
{
    trigger_channel_is_open = 1;
    TriggerChannel = new TRC_channel(dir_path, max_n_files, ch_id, file_midname, First_File);
}


bool TRC_FileReader::GetNextEvent()
{

    for(int i = 0; i < Channels.size(); ++i)
    {
        if(Channels.at(i).GetNextEvent())
        {
            std::cout << "End of events" << std::endl;
            return 0;
        }
        else if( trigger_channel_is_open)
        {
            TriggerChannel->GetNextEvent();
            Detectors->SetTriggerNumber( CalculateTriggerNumber() );
        }

        
        Detectors->SetDetectorWaveform(i,Channels.at(i).GetWaveformX(),Channels.at(i).GetWaveformY());
    }
    return 1;
}

void TRC_FileReader::SetDetectorSetup(DetectorSetup &det)
{
    Detectors = &det;
}

int TRC_FileReader::CalculateTriggerNumber()
{
   double sample;
    int number, bit, kk, inipulse, endpulse, nsamples, step;
    std::vector<double> trigger_wave_x = TriggerChannel->GetWaveformX();
    std::vector<double> trigger_wave_y = TriggerChannel->GetWaveformY();
    double fSampleTime = (trigger_wave_x.at(1) - trigger_wave_x.at(0))*1.e+9;
    number = 0; bit = 1;

    nsamples = trigger_wave_x.size();
    endpulse = nsamples / 2;
    inipulse = (int)(1480.0 / fSampleTime);
    step = (int)(25.0/fSampleTime);

    kk = inipulse;
    while(kk > endpulse)
    {
      sample = trigger_wave_y.at(kk);
      if(sample < -0.5) // Last year 1.2, now is 0.5
        number += bit;

      bit = bit*2;
      kk = kk - step;
    }

    // The SRS event number is returned.
    return ((number-1)/2); 

}
