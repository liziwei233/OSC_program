#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <vector>

#include <TBenchmark.h>

#include "AverageTool.h"
#include "FileReader.h"
#include "Detector.h"
#include "MuonSetup.h"
#include "TrackInfo.h"

int main(int argc, char* argv[])
{
    //1st input argument is filename with waveforms
    //2nd input argument is filename with tracks
    //3rd input argument is name the output root file will have

    AverageTool aver(4096);
    
    int enable_dumping = 0;
    if(argc > 4)
    {
        if(atoi(argv[4]) == 1)
            enable_dumping = 1;
    }

    TBenchmark bench;
    bench.Start("full");

    const char* output_rootfile_name = argv[3];
    //const char* output_rootfile_name = "ignore_this.root";
    //TrackInfo Tracker(argv[2]);

    TrackInfo Tracker(argv[2]);

    FileReader myfile(argv[1]);

    MuonSetup mysetup;
    mysetup.CreateMM();
    mysetup.CreateMCP();
    mysetup.CreateMCP();

    int NumberofTracks;
    Track OneTrack;
    mysetup.init(NumberofTracks, OneTrack);
    myfile.SetDetectorSetup(mysetup);

    int dumping_id = 0;

    std::cout << std::endl;
    int i = 0;
    //FileReader myfile(argv[1]);
    //=======================
    //std::vector<int> channel_IDs;
    //channel_IDs.push_back(3); //<- Channel 3 (Picosecond Micromegas)
    //channel_IDs.push_back(2); //<- Channel 2 (Photodiode)
    //TRC_FileReader myfile(channel_IDs,argv[1],atoi(argv[2]));

    //DetectorSetup mysetup;
    //mysetup.CreateMM();
    //mysetup.CreateMCP();

    ////mysetup.init(NumberofTracks, OneTrack);
    //mysetup.init_laser();
    //myfile.SetDetectorSetup(mysetup);

    //int dumping_id = 0;

    //std::cout << std::endl;
    //int i = 0;
    //======================
    //while( myfile.GetNextEvent() )
    while( 1 ) 
    {

        int TriggerNumber;
        if(myfile.GetNextEvent(++i, TriggerNumber)) 
        {
            std::cout << "EOF" << std::endl;
            break;
        }
        mysetup.SetTriggerNumber(TriggerNumber);

        NumberofTracks = Tracker.NumberofTracks[TriggerNumber];
        OneTrack = Tracker.TrackBank[TriggerNumber].at(0);

        printf("\rEvent is: %.5d ===============================================",i-1);//magic 
        mysetup.SetWaveformToAverage(aver);
        aver.StandardAverage();
        //mysetup.AverageAnalysis();
        //mysetup.StandardAnalysis();
        
        
        //if(i-1==dumping_id) // Specify Event to be dumped to dumpfile.root (read by executing ` root -l draw_dump.C `)
        //    mysetup.Dump();

        if(dumping_id >=0 && enable_dumping) 
            if(dumping_id<=i-1)
            {
                printf("\nNext event to dump: (give negative to stop dumping)");
                int ok = scanf("%d",&dumping_id);
                if(dumping_id < 0) break;
            }

        //mysetup.Fill_Tree();
        //if(i==1) break;
    }
    bench.Show("full");
    aver.Finalize();
    aver.Write(output_rootfile_name);
    mysetup.Finalize_Tree("delete.this");
    
    std::cout << "done" << std::endl;
}

