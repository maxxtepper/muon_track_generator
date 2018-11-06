#ifndef InitConditions_3D_h
#define InitConditions_3D_h

#include <stdlib.h>
#include <cstdint>

class InitConditions_3D {
	public:
		InitConditions_3D(uint32_t);
		
		bool AddValues(double,double,double,double,double,double);

		~InitConditions_3D();

		double *x0, *y0, *z0;
		double *vx0, *vy0, *vz0;
	private:
		uint32_t _index;
		uint32_t _N;
};

#endif
