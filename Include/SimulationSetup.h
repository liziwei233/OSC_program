#ifndef __lightersetup_h__
#define __lightersetup_h__

#include "DetectorSetup.h"

class SimulationSetup : public DetectorSetup
{

    public:
        void SimulationAnalysis();
        void SetWaveformToAverage(AverageTool &aver);
        void Dump();
        void init();
        void init_tree();
};

#endif
