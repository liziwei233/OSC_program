#ifndef __trc_filereader_h__
#define __trc_filereader_h__

#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <string.h>

#include "Detector.h"
#include "DetectorSetup.h"

class Simulation_FileReader
{
    public:
        Simulation_FileReader(const char* file_name);
        bool GetNextEvent();
        void SetDetectorSetup(DetectorSetup &det);
    private:
        FILE* input_file;
        DetectorSetup* Detectors;
};
#endif




