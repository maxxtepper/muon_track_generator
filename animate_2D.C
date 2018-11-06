#include <stdio.h>
#include <fstream>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TBranch.h>
#include <TGraph.h>
#include <TGraph2D.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TStyle.h>
#include <TPolyMarker3D.h>
#include <TAxis.h>
#include <TSystem.h>
#include <TPolyLine3D.h>
#include <TH3D.h>
#include <TH2D.h>

typedef struct {
	Double_t x, y, z, r, p, t;
} PARTICLE;

void animate_2D(unsigned long N, unsigned long M, unsigned long R) {
	//////////////////////////////////////////////////////
	////		Get pwd for File Saving
	//////////////////////////////////////////////////////
	std::string pwd;
	char cwd[1024];
	if (getcwd(cwd,sizeof(cwd)) != NULL)
		pwd = cwd;
	else {
		printf("ERROR: Could not find cwd!\n");
	}

	//////////////////////////////////////////////////////
	////		ROOT Setup
	//////////////////////////////////////////////////////
	std::string inPref, inF, infileName;
	inPref = pwd + "/raw_root/";

	//	Prepare the chain
	std::string run;
	run = std::to_string(R);
	std::string ch_name = "rawMTracks_run_" + run;
	TChain *chain = new TChain(ch_name.c_str());

	//	Get the file
	inF = "raw_muon_tracks_" + std::to_string(R) + ".root";
	infileName = inPref + inF;
	std::ifstream fin;
	fin.open(infileName.c_str());
	if (!fin.fail()) {
		fin.close();
		chain->Add(infileName.c_str());
	}

	//	Setup the branches (particles)
	PARTICLE *particle[N];
	for (unsigned long i=0; i<N; i++) {
		particle[i] = new PARTICLE();
		std::string br_id = "particle_" + std::to_string(i);
		chain->SetBranchAddress(br_id.c_str(), particle[i]);
	}

	//////////////////////////////////////////////////////
	////		Animation Parameters Setup
	//////////////////////////////////////////////////////
	//	Chain entry setup
	Long_t nEntries = 0;
	nEntries = chain->GetEntries();

	//	Get mins and maxs
	Double_t x_min=0, y_min=0, z_min=0;
	Double_t x_max=0, y_max=0, z_max=0;
	Double_t *r_max;

	unsigned long rBytes = N*sizeof(Double_t);
	r_max = (Double_t*)malloc(rBytes);
	for (unsigned long i=0; i<N; i++) r_max[i] = 0.;

	for (Long_t i=0; i<nEntries; i++) {
		chain->GetEntry(i);
		for (unsigned long j=0; j<N; j++) {
			Double_t hold;
			chain->GetEntry(i);
			hold = particle[j]->x;
			if (hold < x_min) x_min = hold;
			if (hold > x_max) x_max = hold;
			hold = particle[j]->y;
			if (hold < y_min) y_min = hold;
			if (hold > y_max) y_max = hold;
			hold = particle[j]->z;
			if (hold < z_min) z_min = hold;
			if (hold > z_max) z_max = hold;
			hold = particle[j]->r;
			if (hold > r_max[j]) r_max[j] = hold;
		}
	}

	//	Set Detector Radii
	double layer0, layer1, layer2, layer3;
	Int_t layCol0, layCol1, layCol2, layCol3, layCol4;
	Int_t layWid, laySty;
	layer0 = 0.01;
	layer1 = 0.02;
	layer2 = 0.03;
	layer3 = 0.04;
	layCol0 = kOrange-4;
	layCol1 = kYellow-4;
	layCol2 = kGreen-4;
	layCol3 = kBlue-4;
	layCol4 = kViolet-4;
	layWid = 4;
	laySty = 1;

	//////////////////////////////////////////////////////
	////		Animation 0 - Setup
	//////////////////////////////////////////////////////
	//	Setup Canvas, TH3D for visualization
	std::string cv_id = "c0" + std::to_string(R);
	TCanvas *c0 = new TCanvas(cv_id.c_str(),"Muon Tracks",0,0,1920,1040);
	TH3D *h3_0 = new TH3D("h3_0","h3_0",1,x_min,x_max,1,y_min,y_max,1,z_min,z_max);
	h3_0->SetTitle("3D Animation");
	h3_0->GetXaxis()->SetTitle("x(m)");
	h3_0->GetXaxis()->SetTitleOffset(1.4);
	h3_0->GetYaxis()->SetTitle("y(m)");
	h3_0->GetYaxis()->SetTitleOffset(1.8);
	h3_0->GetZaxis()->SetTitle("z(m)");
	h3_0->GetZaxis()->SetTitleOffset(1.);
	gStyle->SetOptStat(0);
	h3_0->Draw();

	//	Create p-p beams
	TPolyLine3D *b[2];
	b[0] = new TPolyLine3D();
	b[0]->SetLineWidth(layWid);
	b[0]->SetLineStyle(laySty);
	b[0]->SetLineColor(2);
	b[1] = new TPolyLine3D();
	b[1]->SetLineWidth(layWid);
	b[1]->SetLineStyle(laySty);
	b[1]->SetLineColor(2);

	//	Create muon tracks
	TPolyLine3D *l0[N];
	for (unsigned int j=0; j<N; j++) {
		l0[j] = new TPolyLine3D();
		l0[j]->SetLineWidth(layWid);
		l0[j]->SetLineStyle(laySty);
		if (r_max[j]<layer0)
			l0[j]->SetLineColor(layCol0);
		if ((r_max[j]>=layer0) && (r_max[j]<layer1))
			l0[j]->SetLineColor(layCol1);
		if ((r_max[j]>=layer1) && (r_max[j]<layer2))
			l0[j]->SetLineColor(layCol2);
		if ((r_max[j]>=layer2) && (r_max[j]<layer3))
			l0[j]->SetLineColor(layCol3);
		if (r_max[j]>=layer3)
			l0[j]->SetLineColor(layCol4);
	}

	//////////////////////////////////////////////////////
	////		Animation 0 - 3D Muon Tracks
	//////////////////////////////////////////////////////
	//	Animate p-p beams
	unsigned long pnt_cnt = 0;
	printf("Drawing run %ld\n",R);
	Double_t ent = nEntries/4;
	Double_t I = 0.;
	for (Long_t i=0; i<(ent+5); i++) {
		if (!i) {
			b[0]->Draw();
			b[1]->Draw();
		}
		Double_t x_0 = x_min*(1-(I/ent));
		Double_t x_1 = x_max*(1-(I/ent));
		if (i>=ent) x_0 = 0;
		if (i>=ent) x_1 = 0;
		Int_t pnt = i%5;
		b[0]->SetPoint(pnt,x_0,0.,0.);
		b[1]->SetPoint(pnt,x_1,0.,0.);
		pnt_cnt++;
		if (i) {
			c0->Modified();
			c0->Update();
		}
		I++;
	}

	//	Cleanup p-p beams
	//b[0]->Delete();
	//b[1]->Delete();
	c0->Modified();
	c0->Update();

	//	Animate muon tracks
	pnt_cnt = 0;
	printf("Drawing run %ld\n",R);
	for (Long_t i=0; i<nEntries; i++) {
		chain->GetEntry(i);
		for (unsigned long j=0; j<N; j++) {
			if (!i) l0[j]->Draw();
			Int_t pnt = i%10;
			l0[j]->SetPoint(pnt,particle[j]->x, particle[j]->y,particle[j]->z);
		}
		pnt_cnt++;
		c0->Modified();
		c0->Update();
	}
	sleep(2);

	//////////////////////////////////////////////////////
	////		Animation 1 - Setup
	//////////////////////////////////////////////////////
	//	Re-Create Canvas
	std::string cv1_id = "c1" + std::to_string(R);
	TCanvas *c1 = new TCanvas(cv1_id.c_str(),"Muon Tracks",0,0,1920,1040);

	//	Prepare canvas and pads
	c1->Divide(2,2,0.001,0.001);

	c1->cd(1);
	TH2D *h2_0 = new TH2D("h2_0","h2_0",100,x_min,x_max,100,y_min,y_max);
	h2_0->SetTitle("XY Projection");
	h2_0->GetXaxis()->SetTitle("x(m)");
	h2_0->GetYaxis()->SetTitle("y(m)");
	gStyle->SetOptStat(0);
	h2_0->Draw();

	c1->cd(2);
	TH2D *h2_1 = new TH2D("h2_1","h2_1",100,x_min,x_max,100,z_min,z_max);
	h2_1->SetTitle("XZ Projection");
	h2_1->GetXaxis()->SetTitle("x(m)");
	h2_1->GetYaxis()->SetTitle("z(m)");
	gStyle->SetOptStat(0);
	h2_1->Draw();
	
	c1->cd(3);
	TH2D *h2_2 = new TH2D("h2_2","h2_2",100,y_min,y_max,100,z_min,z_max);
	h2_2->SetTitle("YZ Projection");
	h2_2->GetXaxis()->SetTitle("y(m)");
	h2_2->GetYaxis()->SetTitle("z(m)");
	gStyle->SetOptStat(0);
	h2_2->Draw();

	c1->cd(4);
	TH3D *h3_1 = new TH3D("h3_1","h3_1",100,x_min,x_max,100,y_min,y_max,100,z_min,z_max);
	h3_1->SetTitle("3D Animation");
	h3_1->GetXaxis()->SetTitle("x(m)");
	h3_1->GetXaxis()->SetTitleOffset(1.4);
	h3_1->GetYaxis()->SetTitle("y(m)");
	h3_1->GetYaxis()->SetTitleOffset(1.8);
	h3_1->GetZaxis()->SetTitle("z(m)");
	h3_1->GetZaxis()->SetTitleOffset(1.);
	gStyle->SetOptStat(0);
	h3_1->Draw();

	//	Create 2D graphs for projections
	TGraph *g0[N], *g1[N], *g2[N];
	for (unsigned long i=0; i<N; i++) {
		g0[i] = new TGraph();
		g0[i]->SetLineWidth(2);
		g0[i]->SetLineStyle(1);
		g1[i] = new TGraph();
		g1[i]->SetLineWidth(2);
		g1[i]->SetLineStyle(1);
		g2[i] = new TGraph();
		g2[i]->SetLineWidth(2);
		g2[i]->SetLineStyle(1);
		if (r_max[i]<layer0) {
			g0[i]->SetLineColor(layCol0);
			g1[i]->SetLineColor(layCol0);
			g2[i]->SetLineColor(layCol0);
		}
		if ((r_max[i]>=layer0) && (r_max[i]<layer1)) {
			g0[i]->SetLineColor(layCol1);
			g1[i]->SetLineColor(layCol1);
			g2[i]->SetLineColor(layCol1);
		}
		if ((r_max[i]>=layer1) && (r_max[i]<layer2)) {
			g0[i]->SetLineColor(layCol2);
			g1[i]->SetLineColor(layCol2);
			g2[i]->SetLineColor(layCol2);
		}
		if ((r_max[i]>=layer2) && (r_max[i]<layer3)) {
			g0[i]->SetLineColor(layCol3);
			g1[i]->SetLineColor(layCol3);
			g2[i]->SetLineColor(layCol3);
		}
		if (r_max[i]>=layer3) {
			g0[i]->SetLineColor(layCol4);
			g1[i]->SetLineColor(layCol4);
			g2[i]->SetLineColor(layCol4);
		}
	}

	//	Re-Create muon tracks
	TPolyLine3D *l1[N];
	for (unsigned int j=0; j<N; j++) {
		l1[j] = new TPolyLine3D();
		l1[j]->SetLineWidth(layWid);
		l1[j]->SetLineStyle(laySty);
		if (r_max[j]<layer0)
			l1[j]->SetLineColor(layCol0);
		if ((r_max[j]>=layer0) && (r_max[j]<layer1))
			l1[j]->SetLineColor(layCol1);
		if ((r_max[j]>=layer1) && (r_max[j]<layer2))
			l1[j]->SetLineColor(layCol2);
		if ((r_max[j]>=layer2) && (r_max[j]<layer3))
			l1[j]->SetLineColor(layCol3);
		if (r_max[j]>=layer3)
			l1[j]->SetLineColor(layCol4);
	}
	free(r_max);

	//////////////////////////////////////////////////////
	////		Animation 1 - 3D Muon Tracks and 2D Projections
	//////////////////////////////////////////////////////
	for (Long_t i=0; i<nEntries; i++) {
		chain->GetEntry(i);
		for (unsigned long j=0; j<N; j++) {
			if (!i) {
				c1->cd(1);
				g0[j]->Draw("SAME");
				c1->cd(2);
				g1[j]->Draw("SAME");
				c1->cd(3);
				g2[j]->Draw("SAME");
				c1->cd(4);
				l1[j]->Draw();
			}
			g0[j]->SetPoint(pnt_cnt,particle[j]->x,particle[j]->y);
			g1[j]->SetPoint(pnt_cnt,particle[j]->x,particle[j]->z);
			g2[j]->SetPoint(pnt_cnt,particle[j]->y,particle[j]->z);
			l1[j]->SetPoint(pnt_cnt,particle[j]->x,particle[j]->y,particle[j]->z);
		}
		pnt_cnt++;
		for (int k=1; k<5; k++) {
			c1->cd(k);
			gPad->Modified();
			gPad->Update();
		}
	}
	sleep(2);
}
