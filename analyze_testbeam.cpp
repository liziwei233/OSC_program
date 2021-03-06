#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <vector>

#include <TBenchmark.h>

#include "TRC_FileReader.h"
#include "Detector.h"
#include "TestBeamSetup.h"
#include "TrackInfo.h"

TFile *gFileJohn = NULL;
TFile *gFilePre = NULL;
int main(int argc, char *argv[])
{
    //1st input argument is is directory path with TRC files
    //2nd input argument is number of files for each channel in the directory
    //3rd input argument is name the output root file will have
    //4th input argument (optional), if it is 1 then dumping will be enabled

    TBenchmark bench;
    bench.Start("full");

    TrackInfo *Tracker;

    int enable_dumping = 0;
    int dumping_id = 0;
    int eventN = 100;
    int file_id = 0;
    if (argc > 4)
    {
        //if(atoi(argv[4]) == 1)
        //enable_dumping = 1;
        dumping_id = atoi(argv[4]);
        file_id = dumping_id / eventN;
    }
    //Specify the channels written in the files
    std::vector<int> channel_IDs;
    TestBeamSetup mysetup;
    channel_IDs.push_back(1); //<- Channel 2 (Picosecond Micromegas)
    mysetup.CreateTR();
    channel_IDs.push_back(2); //<- Channel 1 (mcp1)
   // channel_IDs.push_back(3); //<- Channel 2 (mcp2)
    //channel_IDs.push_back(4); //<- Channel 2 (mcp2)
    mysetup.CreateMCP();
   // mysetup.CreateMCP();
   // mysetup.CreateMCP();
    //mysetup.CreateMCP();
                              //   TRC_FileReader myfile(channel_IDs,argv[1],atoi(argv[2]), "--trace--");
    //TRC_FileReader myfile(channel_IDs,argv[1],atoi(argv[2]), "--A1A2B1B2--",0);
    //    TRC_FileReader myfile(channel_IDs,argv[1],atoi(argv[2]), "0818test_th30mv",0);
    //TRC_FileReader myfile(channel_IDs,argv[1],atoi(argv[2]), "trace",0);
    TRC_FileReader myfile(channel_IDs, argv[1], atoi(argv[2]), "--Trace--", file_id);
    //    myfile.OpenTriggerChannel(argv[1],atoi(argv[2]),"--trace--",4);


    //mysetup.CreateMCP();
    //    mysetup.CreateMCP();
    //    mysetup.CreateMCP();

    //    mysetup.CreateMM();

    int NumberofTracks;
    Track OneTrack;
    if(argc >5){
    std::cout << argv[5] << "\t" << atoi(argv[5]) << std::endl;
    if (atoi(argv[5]) != 0)
    {
        Tracker = new TrackInfo(argv[6]);
        mysetup.init(channel_IDs,NumberofTracks, OneTrack);
    }
    }
    else
        mysetup.init(channel_IDs);

    myfile.SetDetectorSetup(mysetup);

    const char *output_rootfile_name = argv[3];

    ////////////////////////////////////
    std::cout << std::endl;
    int i = 0;

    gFilePre = gFile;
    gFileJohn = new TFile("FitGraph.root", "recreate");
    while (myfile.GetNextEvent())
    {
        i++;

        printf("\rEvent is: %.5d ===============================================", file_id*eventN+i-1); //magic

        //std::cout << "trignum = " << mysetup.TriggerNumber << std::endl;
        if (argc > 5 && atoi(argv[5]) != 0)
        {
            NumberofTracks = Tracker->NumberofTracks[mysetup.TriggerNumber];
            OneTrack = Tracker->TrackBank[mysetup.TriggerNumber].at(0);
        }

        gFile = gFileJohn;

        mysetup.TestBeamAnalysis();
        gFile = gFilePre;

        if (file_id * eventN + i - 1 == dumping_id) // Specify Event to be dumped to dumpfile.root (read by executing ` root -l draw_dump.C `)
            mysetup.Dump(file_id * eventN + i - 1);
        /* 
        if(dumping_id >=0 && enable_dumping) 
            if(dumping_id<=i-1)
            {
                printf("\nNext event to dump: (give negative to stop dumping)");
                int ok = scanf("%d",&dumping_id);
                if(dumping_id < 0) break;
            }
        */
        mysetup.Fill_Tree();
        //if(i==1) break;
    }
    mysetup.Finalize_Tree(output_rootfile_name);

    gFileJohn->Close();

    bench.Show("full");
}
