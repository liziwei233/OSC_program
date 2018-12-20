#ifndef __muonsetup_h__
#define __muonsetup_h__

#include "DetectorSetup.h"

class MuonSetup : public DetectorSetup
{
    public:
        void MuonAnalysis();
        void SetWaveformToAverage(AverageTool &aver);
        void init();
        void init(int& NumberofTracks, Track& OneTrack);
        void init_tree();
        void tree_add_tracks(int& NumberofTracks, Track& OneTrack);
        void Dump();
};
#endif
