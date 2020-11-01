#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <vector>

#include <TBenchmark.h>

#include "AverageTool.h"
#include "TRC_FileReader.h"
#include "Detector.h"
#include "TestBeamSetup.h"
#include "TrackInfo.h"

int main(int argc, char *argv[])
{
    //1st input argument is is directory path with TRC files
    //2nd input argument is number of files for each channel in the directory
    //3rd input argument is name the output root file will have
    //4th input argument (optional), if it is 1 then dumping will be enabled

    //AverageTool aver(4002);

    int enable_dumping = 0;
    int file_id = 0;
    int dumping_id = 0;
    if (argc > 4)
    {
        //if(atoi(argv[4]) == 1)
        enable_dumping = 1;
        dumping_id = atoi(argv[4]);
    }

    TBenchmark bench;
    bench.Start("full");

    const char *aver_rootfile_name = argv[3];
    const char *output_rootfile_name = "ignore_this.root";
    //TrackInfo Tracker(argv[2]);

    //FileReader myfile(argv[1]);
    std::vector<int> channel_IDs;
    //channel_IDs.push_back(2); //<- Channel 4 (Picosecond Micromegas)
    //channel_IDs.push_back(1); //<- Channel 1 (MCP)
    //channel_IDs.push_back(2); //<- Channel 2 (MCP)
    //TRC_FileReader myfile(channel_IDs,argv[1],atoi(argv[2]),"Trace");
    TestBeamSetup mysetup;
    //mysetup.CreateMM();
    //mysetup.CreateMCP();
    //mysetup.CreateMCP();
    //mysetup.CreateMM();
    channel_IDs.push_back(1); //<- Channel 2 (Picosecond Micromegas)
    mysetup.CreateTR();
    channel_IDs.push_back(2); //<- Channel 2 (Picosecond Micromegas)
   // channel_IDs.push_back(3); //<- Channel 1 (mcp1)
    //channel_IDs.push_back(4); //<- Channel 2 (mcp2)
    mysetup.CreateMCP();
    //mysetup.CreateMCP();
   // mysetup.CreateMCP();
    //mysetup.CreateMCP();
    //avers.push_back(new AverageTool());
    //avers.push_back(new AverageTool());
    //avers.push_back(new AverageTool());
    TRC_FileReader myfile(channel_IDs, argv[1], atoi(argv[2]), "--Trace--", file_id);

    //mysetup.init(NumberofTracks, OneTrack);
    mysetup.init(channel_IDs);
    mysetup.CreateAverageTools();
    myfile.SetDetectorSetup(mysetup);

    int i = 0;
    while (myfile.GetNextEvent())
    {
        printf("\rEvent is: %.5d ===============================================", i); //magic
        //if (i == dumping_id)
        //{

            mysetup.SetWaveformToAverage();

            //aver.StandardAverage();
            //mysetup.AverageAnalysis();
            //mysetup.StandardAnalysis();

            //if(i-1==dumping_id) // Specify Event to be dumped to dumpfile.root (read by executing ` root -l draw_dump.C `)
            //    mysetup.Dump();

            if (i == dumping_id && enable_dumping)
                mysetup.Dump(dumping_id);
            /*
            if(dumping_id<=i-1)
            {
                printf("\nNext event to dump: (give negative to stop dumping)");
                int ok = scanf("%d",&dumping_id);
                if(dumping_id < 0) break;
            }
            */
        //}

        //mysetup.Fill_Tree();
        //if(i==1) break;
        i++;
    }

    mysetup.Finalize_AverageTools(aver_rootfile_name);

    //mysetup.Finalize_Tree(output_rootfile_name);

    bench.Show("full");
    std::cout << "done" << std::endl;
}
