#ifndef MuonTrackGen_h
#define MuonTrackGen_h

#include <stdlib.h>
#include <stdio.h>
#include <cstdint>
#include <math.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <string>
#include <random>
#include <vector>

#include "InitConditions_3D.h"

#define APPROX 0

class MuonTrackGen {
	public:
		MuonTrackGen(uint32_t,uint32_t,double,double,double,double,double);
		~MuonTrackGen();

		void prepareIterator();
		void iterate();

		InitConditions_3D *GetInit_3D() {return init_3D;}

		typedef struct {
			Double_t x, y, z, r, p, t;
		} PARTICLE;

		double *x0 , *y0 , *z0;
		double *vx0, *vy0, *vz0;
		double *v, *gamma, *w_y;

		double time;

		std::vector<PARTICLE*> particle;
		InitConditions_3D *init_3D;

	private:
		//	Constants
		uint32_t _N;
		uint32_t _M;
		double _q;
		double _m;
		double _B;
		double _c;
		double _ts;
		double _w;
};

#endif
