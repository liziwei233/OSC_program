#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <vector>

#include <TBenchmark.h>

#include "Simulation_FileReader.h"
#include "Detector.h"
#include "SimulationSetup.h"
#include "TrackInfo.h"

int main(int argc, char* argv[])
{
    //1st input argument is path of txt file
    //2rd input argument is name the output root file will have
    //3th input argument (optional), if it is 1 then dumping will be enabled
    
    TBenchmark bench;
    bench.Start("full");

    int enable_dumping = 0;
    if(argc > 3)
    {
        if(atoi(argv[3]) == 1)
            enable_dumping = 1;
    }

    //Specify the channels written in the files
    Simulation_FileReader myfile(argv[1]);

    //DetectorSetup mysetup;
    SimulationSetup mysetup;
    mysetup.CreateMM();
    mysetup.init();

    myfile.SetDetectorSetup(mysetup);
    
    const char* output_rootfile_name = argv[2];
    
    int dumping_id = 0;

    std::cout << std::endl;
    int i = 0;
    while( myfile.GetNextEvent() )
    {
        i++;

        printf("\rEvent is: %.5d ===============================================",i-1);//magic 
        mysetup.SimulationAnalysis();
        
        
        if(i-1==dumping_id) // Specify Event to be dumped to dumpfile.root (read by executing ` root -l draw_dump.C `)
            mysetup.Dump();
        
        if(dumping_id >=0 && enable_dumping) 
            if(dumping_id<=i-1)
            {
                printf("\nNext event to dump: (give negative to stop dumping)");
                int ok = scanf("%d",&dumping_id);
                if(dumping_id < 0) break;
            }

        mysetup.Fill_Tree();
        //if(i==1) break;
    }
    mysetup.Finalize_Tree(output_rootfile_name);
    
    bench.Show("full");
}

