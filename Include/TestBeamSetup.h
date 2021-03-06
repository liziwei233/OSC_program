#ifndef __DetectorSetup_h__
#define __DetectorSetup_h__

#include "DetectorSetup.h"
#include "TrackInfo.h"

class TestBeamSetup : public DetectorSetup
{
    public:
        ~TestBeamSetup();
        void TestBeamAnalysis();
        void CreateAverageTools();
        void SetWaveformToAverage();
        void Finalize_AverageTools(const char* outfile_name);
        void Dump(int id);
        void init(std::vector<int> channel_ids);
        void init_tree();
        void tree_add_tracks(int& NumberofTracks, Track& OneTrack);
        void init(std::vector<int> channel_ids,int& NumberofTracks, Track& OneTrack);
    protected:
        int record_blregion_end[10];
        std::vector<int> Channel_IDs;
        std::vector<AverageTool*> avers;
};
#endif
