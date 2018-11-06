#include "MuonTrackGen.h"

MuonTrackGen::MuonTrackGen(uint32_t N, uint32_t M, double q, double m, double B, double c, double ts) {
	_N  = N;
	_M  = M;
	_q  = q;
	_m  = m;
	_B  = B;
	_c  = c;
	_ts = ts;

	_w = q*B/m;

	//	Allocate memory
	uint32_t bytes = N*sizeof(double);
	x0    = (double*)malloc(bytes);
	y0    = (double*)malloc(bytes);
	z0    = (double*)malloc(bytes);
	vx0   = (double*)malloc(bytes);
	vy0   = (double*)malloc(bytes);
	vz0   = (double*)malloc(bytes);
	v     = (double*)malloc(bytes);
	gamma = (double*)malloc(bytes);
	w_y   = (double*)malloc(bytes);

	//	Allocate Particle objects
	for (uint32_t i=0; i<_N; i++) {
		PARTICLE *prtcl = new PARTICLE();
		particle.push_back(prtcl);
	}

	//	Allocate init condition object
	init_3D = new InitConditions_3D(_N);
}

MuonTrackGen::~MuonTrackGen() {
	free(x0);
	free(y0);
	free(z0);
	free(vx0);
	free(vy0);
	free(vz0);
	free(v);
	free(gamma);
	free(w_y);

	for (uint32_t i=0; i<_N; i++)
		delete particle[i];

	delete init_3D;
}

void MuonTrackGen::prepareIterator() {
	for (uint32_t i=0; i<_N; i++) {
		v[i]     = sqrt(pow(init_3D->vx0[i],2)+pow(init_3D->vy0[i],2)+pow(init_3D->vz0[i],2));
		gamma[i] = 1/sqrt(1-(pow((v[i]/_c),2)));
		w_y[i]   = _w/gamma[i];
	}
	
	time = 0.0f;
}

void MuonTrackGen::iterate() {
	//	Prepare for this time step
	time += _ts;
	//	Calculate this time step...
	for (uint32_t i=0; i<_N; i++) {
		particle[i]->z = ((init_3D->x0[i]*cos(w_y[i]*time))+((init_3D->vx0[i]/w_y[i])*sin(w_y[i]*time)));
		particle[i]->y = ((-1)*init_3D->x0[i]*sin(w_y[i]*time))+((init_3D->vx0[i]/w_y[i])*(cos(w_y[i]*time)))+((init_3D->vy0[i]+w_y[i]*init_3D->x0[i])*time) + (init_3D->y0[i]-(init_3D->vx0[i]/w_y[i]));
		particle[i]->x = (init_3D->vz0[i]*time);
		particle[i]->r = sqrt(particle[i]->x*particle[i]->x+particle[i]->y*particle[i]->y); 
		particle[i]->p = atan(particle[i]->y/particle[i]->x);
		particle[i]->t = time;
	}
}
