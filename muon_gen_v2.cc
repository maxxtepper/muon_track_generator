#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <cstdint>
#include <math.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <string>
#include <random>

#include "MuonTrackGen.h"

#define APPROX 0

typedef struct {
	Double_t x, y, z, r, p, t;
} PARTICLE;

int main(int argc, char* argv[]) {
	//////////////////////////////////////////////////////
	////		Check Input Parameters
	//////////////////////////////////////////////////////
	if (argc != 6) {
		printf("Usage: ./arrady_time_inc_test base mag N M R\n\
				base = base of time step\n\
				mag  = magnitude of time step (-)\n\
				N    = number of particles\n\
				M    = number of time steps\n\
				R    = number of runs\n");
		return -1;
	}

	//////////////////////////////////////////////////////
	////		Initialize Input Parameters
	//////////////////////////////////////////////////////
	double base, mag;
	uint32_t N, M, R;
	base = atof(argv[1]);
	mag  = atof(argv[2]);
	N    = atol(argv[3]);
	M    = atol(argv[4]);
	R    = atol(argv[5]);

	uint32_t bytes = N*sizeof(double);

	//////////////////////////////////////////////////////
	////		Create Random Number Generator
	//////////////////////////////////////////////////////
	std::random_device rd;
  std::mt19937_64 gen(rd());

	//////////////////////////////////////////////////////
	////		Get pwd for File Saving
	//////////////////////////////////////////////////////
	std::string pwd;
	char cwd[1024];
	if (getcwd(cwd,sizeof(cwd)) != NULL)
		pwd = cwd;
	else {
		printf("ERROR: Could not find cwd!\n");
		return -1;
	}
	
	//////////////////////////////////////////////////////
	////		Constant Assignments
	//////////////////////////////////////////////////////
	double q  = 1.6e-19;
	double m  = 1.8e-28;
	double B  = 3.8f;
	double c  = 2.99792458e8;
	double ts = base*pow(10,((-1)*mag));

	//////////////////////////////////////////////////////
	////		Create Muon Track Generator
	//////////////////////////////////////////////////////
	MuonTrackGen *mtg;
	mtg = new MuonTrackGen(N,M,q,m,B,c,ts);

	//////////////////////////////////////////////////////
	////		ROOT Setup
	//////////////////////////////////////////////////////
	std::string outPref, outfileName;
	outPref = pwd + "/raw_root/";
	outfileName = outPref + "raw_muon_tracks_" + std::to_string(R) + ".root";

	//	Create file
	TFile *outFile;
	outFile = TFile::Open(outfileName.c_str(), "RECREATE");

	//	Create tree (run)
	std::cout << "Generating run " << R << std::endl;
	std::string rT_id   = "rawMTracks_run_" + std::to_string(R);
	std::string rT_name = "Raw Data of Muon Track - Run " + std::to_string(R);
	TTree *rawT = new TTree(rT_id.c_str(),rT_name.c_str());

	//	Create branches (particles)
	TBranch *branch[N];
	for (uint32_t i=0; i<N; i++) {
		//	Create a branch for each particle in the run
		branch[i] = new TBranch();
		std::string br_id   = "particle_" + std::to_string(i);
		branch[i] = rawT->Branch(br_id.c_str(), mtg->particle[i], "x/D:y/D:z/D:r/D:p/D:t/D");
	}

	//////////////////////////////////////////////////////
	////		Iterator Setup
	//////////////////////////////////////////////////////
	for (uint32_t i=0; i<N; i++) {

		double x0 , y0 , z0;
		double vx0, vy0, vz0;

		//	Set initial position for particle
		x0 = 0.0f;
		y0 = 0.0f;
		z0 = 0.0f;
	
		//	Generate pseudo-random initial velocity
		double norm = 1.0f;

		std::uniform_real_distribution<> dis0(-0.4,0.4);
		double A = dis0(gen);
		norm -= abs(A);
		vz0 = c*sqrt(abs(A));
		if (A<0) vz0 *= (-1);

		std::uniform_real_distribution<> dis1((-1)*norm,norm);
		double B = dis1(gen);
		norm -= abs(B);
		vy0 = c*sqrt(abs(B));
		if (B<0) vy0 *= (-1);

		std::uniform_real_distribution<> dis2((-1)*norm,norm);
		double C = dis2(gen);
		std::uniform_real_distribution<> dis3(0.99,0.999999);
		double D = dis3(gen);
		vx0 = D*sqrt(pow(c,2)-pow(vy0,2)-pow(vz0,2));
		if (C<0) vx0 *= (-1);

		//	Add the values to the init conditions object
		mtg->GetInit_3D()->AddValues(x0,y0,z0,vx0,vy0,vz0);
	}

	//////////////////////////////////////////////////////
	////		Prepare the Iterator
	//////////////////////////////////////////////////////
	mtg->prepareIterator();

	//////////////////////////////////////////////////////
	////		Run the Iterator
	//////////////////////////////////////////////////////
	//	Run for M time steps
	for (uint32_t i=0; i<M; i++) {
		//	Generate data for this time step
		mtg->iterate();
		//	Save this time step to the tree
		rawT->Fill();
	}

	//////////////////////////////////////////////////////
	////		Finalize ROOT
	//////////////////////////////////////////////////////
	rawT->Write();
	outFile->Close();

	return 0;
}
