OBJDIR = Object
SRCDIR = Source
INCDIR = Include

HEADERS = $(wildcard $(INCDIR)/*.h)
OBJECTS = $(subst $(INCDIR), $(OBJDIR), $(HEADERS:.h=.o))

CC = g++
CFLAGS = -O2 -std=c++11 `root-config --libs --cflags` \
		 -I$(INCDIR)

LDFLAGS = `root-config --glibs` 

all: $(OBJECTS) analyze_simulation analyze_muons analyze_laser analyze_calibration average_muons average_muons average_calibration analyze_testbeam average_testbeam
muons: $(OBJECTS) analyze_muons average_muons
laser: $(OBJECTS) analyze_laser average_laser
calibration: $(OBJECTS) analyze_calibration average_calibration
sim: $(OBJECTS) analyze_simulation average_simulation 
testbeam: $(OBJECTS) analyze_testbeam average_testbeam 
lab: $(OBJECTS) analyze_testbeam

average_laser: $(OBJECTS) average_laser.cpp 
	@echo "Building executable 'average_laser'..."
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

analyze_testbeam: $(OBJECTS) analyze_testbeam.cpp 
	@echo "Building executable 'analyze_testbeam'..."
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

average_testbeam: $(OBJECTS) average_testbeam.cpp 
	@echo "Building executable 'average_testbeam'..."
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

average_muons: $(OBJECTS) average_muons.cpp 
	@echo "Building executable 'average_muons'..."
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

average_calibration: $(OBJECTS) average_calibration.cpp 
	@echo "Building executable 'average_calibration'..."
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

average_simulation: $(OBJECTS) average_simulation.cpp 
	@echo "Building executable 'average_simulation'..."
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

analyze_muons: $(OBJECTS) analyze_muons.cpp 
	@echo "Building executable 'analyze_muons'..."
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

analyze_laser: $(OBJECTS) analyze_laser.cpp 
	@echo "Building executable 'analyze_laser'..."
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

analyze_calibration: $(OBJECTS) analyze_calibration.cpp 
	@echo "Building executable 'analyze_calibration'..."
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

analyze_simulation: $(OBJECTS) analyze_simulation.cpp 
	@echo "Building executable 'analyze_simulation'..."
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJDIR)/Detector.o: $(SRCDIR)/Detector.cpp $(INCDIR)/Detector.h 
	@echo "Building "$@"..."
	@$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/DetectorSetup.o: $(SRCDIR)/DetectorSetup.cpp $(INCDIR)/DetectorSetup.h 
	@echo "Building "$@"..."
	@$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/MuonSetup.o: $(SRCDIR)/MuonSetup.cpp $(INCDIR)/MuonSetup.h 
	@echo "Building "$@"..."
	@$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/LaserSetup.o: $(SRCDIR)/LaserSetup.cpp $(INCDIR)/LaserSetup.h 
	@echo "Building "$@"..."
	@$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/TestBeamSetup.o: $(SRCDIR)/TestBeamSetup.cpp $(INCDIR)/TestBeamSetup.h 
	@echo "Building "$@"..."
	@$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/CalibrationSetup.o: $(SRCDIR)/CalibrationSetup.cpp $(INCDIR)/CalibrationSetup.h 
	@echo "Building "$@"..."
	@$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/SimulationSetup.o: $(SRCDIR)/SimulationSetup.cpp $(INCDIR)/SimulationSetup.h 
	@echo "Building "$@"..."
	@$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/FileReader.o: $(SRCDIR)/FileReader.cpp $(INCDIR)/FileReader.h 
	@echo "Building "$@"..."
	@$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/TRC_FileReader.o: $(SRCDIR)/TRC_FileReader.cpp $(INCDIR)/TRC_FileReader.h 
	@echo "Building "$@"..."
	@$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/TRC_channel.o: $(SRCDIR)/TRC_channel.cpp $(INCDIR)/TRC_channel.h 
	@echo "Building "$@"..."
	@$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/Simulation_FileReader.o: $(SRCDIR)/Simulation_FileReader.cpp $(INCDIR)/Simulation_FileReader.h 
	@echo "Building "$@"..."
	@$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/TrackInfo.o: $(SRCDIR)/TrackInfo.cpp $(INCDIR)/TrackInfo.h 
	@echo "Building "$@"..."
	@$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/tracks.o: $(SRCDIR)/tracks.C $(INCDIR)/tracks.h 
	@echo "Building "$@"..."
	@$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/AverageTool.o: $(SRCDIR)/AverageTool.cpp $(INCDIR)/AverageTool.h 
	@echo "Building "$@"..."
	@$(CC) -c $(CFLAGS) $< -o $@

clean: 
	rm $(OBJDIR)/*.o
