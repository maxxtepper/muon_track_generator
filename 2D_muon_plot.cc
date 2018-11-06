#include <iostream>
#include <stdio.h>
#include <fstream>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TBranch.h>
#include <TGraph2D.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TStyle.h>
#include <TPolyMarker3D.h>
#include <TAxis.h>

typedef struct {
	Double_t x, y, z, r, p, t;
} PARTICLE;

int main(int argc, char *argv[]) {
	if (argc != 4) {
		printf("Usage: ./arrady_time_inc_test N M\n\
				N = number of particles\n\
				M = number of time steps\n\
				R = number of runs\n");
		return -1;
	}

	//	Progradm Paradmeters
	unsigned long N, M, R;
	N = atol(argv[1]);
	M = atol(argv[2]);
	R = atol(argv[3]);
	
	std::string pwd;
	char cwd[1024];
	if (getcwd(cwd,sizeof(cwd)) != NULL)
		pwd = cwd;
	else {
		printf("ERROR: Could not find cwd!\n");
		return -1;
	}

	std::string infileName, outfileName;
	std::string inF, outF;
	std::string inPref, outPref;
	std::string byte;

	inPref = pwd + "/raw_root/";
	outPref = pwd + "/graph2D_root/";
	outF = "graph2D_run_" + std::to_string(R) + ".root";
	outfileName = outPref + outF;

	TFile *outfile;
	outfile = TFile::Open(outfileName.c_str(), "RECREATE");
	
	//	Main variables
	PARTICLE *particle[N];
	for (unsigned long i=0; i<N; i++)
		particle[i] = new PARTICLE();

	//	Get the tree
	//	Call each run
	for (unsigned long k=0; k<R; k++) {
		std::string run;
		run = std::to_string(k);
		std::string ch_name = "rawMTracks_run_" + run;
		TChain *chain = new TChain(ch_name.c_str());

		inF = "raw_muon_tracks.root";
		infileName = inPref + inF;
		std::ifstream fin;
		fin.open(infileName.c_str());
		if (!fin.fail()) {
			fin.close();
			//std::cout << "Adding file " << inF << std::endl;
			chain->Add(infileName.c_str());
		} else break;

		//	Get the branches
		for (unsigned long i=0; i<N; i++) {
			std::string br_id = "particle_" + std::to_string(i);
			chain->SetBranchAddress(br_id.c_str(), particle[i]);
		}

		std::string cv_id = "c" + std::to_string(k);
		TCanvas *c = new TCanvas(cv_id.c_str(),"Muon Tracks",0,0,600,400);
		TGraph2D *dt = new TGraph2D();
//		Float_t *ptr = new Float_t [M*3];
//		TPolyMarker3D *tpm = new TPolyMarker3D(M,ptr,1);
		Long_t nEntries = 0;
		nEntries = chain->GetEntries();
		//	nEntries = # of time steps in each
		//	branch of current tree (run)
		unsigned long pnt_cnt = 0;
		printf("Drawing run %ld\n",k);
		dt->SetMarkerStyle(20);
		dt->GetXaxis()->SetTitle("x (m)");
		dt->GetYaxis()->SetTitle("y (m)");
		dt->GetZaxis()->SetTitle("z (m)");
		dt->Draw("pcol");
		for (Long_t i=0; i<nEntries; i++) {
			//	Gif output

			chain->GetEntry(i);
			for (unsigned long j=0; j<N; j++) {
				dt->SetPoint(pnt_cnt,particle[j]->x, particle[j]->y,particle[j]->z);
				pnt_cnt++;
			}
		}
		c->Modified();
		c->Update();
		c->Write();
	}
	outfile->Close();
	return 0;
}
