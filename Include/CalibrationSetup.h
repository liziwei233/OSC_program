#ifndef __calibrationsetup_h__
#define __calibrationsetup_h__

#include "DetectorSetup.h"

class CalibrationSetup : public DetectorSetup
{

    public:
        void CalibrationAnalysis();
        void SetWaveformToAverage(AverageTool &aver);
        void Dump();
        void init();
        void init_tree();
        void SetCividecWaveformToAverage(AverageTool &aver);
};

#endif
