CC=g++
CFLAGS=-c -std=c++11 -fPIC `root-config --cflags`
LDFLAGS=`root-config --libs`
SOURCES0= InitConditions_3D.cc MuonTrackGen.cc muon_gen_v2.cc
SOURCES1= 2D_muon_plot.cc
OBJECTS0=$(SOURCES0:.cc=.o)
OBJECTS1=$(SOURCES1:.cc=.o)
LIBRARIES=
EXECUTABLE0=muon_gen_v2
EXECUTABLE1=2D_muon_plot

all: $(SOURCES0) $(SOURCES1) $(EXECUTABLE0) $(EXECUTABLE1)

	
$(EXECUTABLE0): $(OBJECTS0)
	$(CC) $(OBJECTS0) -o $@ $(LIBRARIES) $(LDFLAGS) 

$(EXECUTABLE1): $(OBJECTS1)
	$(CC) $(OBJECTS1) -o $@ $(LIBRARIES) $(LDFLAGS) 

.cc.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm ./*~ ./*.o ./*.d ./*.so ./muon_gen_v2 ./2D_muon_plot
