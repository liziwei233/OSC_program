#ifndef __lasersetup_h__
#define __lasersetup_h__

#include "DetectorSetup.h"

class LaserSetup : public DetectorSetup
{
    public:
        void LaserAnalysis();
        void SetWaveformToAverage(AverageTool &aver);
        void Dump();
        void init();
        void init_tree();
};
#endif
