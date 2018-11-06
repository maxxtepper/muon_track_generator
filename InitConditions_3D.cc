#include "InitConditions_3D.h"

InitConditions_3D::InitConditions_3D(uint32_t N) {
	uint32_t bytes = N*sizeof(double);
	_N = N;

	x0    = (double*)malloc(bytes);
	y0    = (double*)malloc(bytes);
	z0    = (double*)malloc(bytes);
	vx0   = (double*)malloc(bytes);
	vy0   = (double*)malloc(bytes);
	vz0   = (double*)malloc(bytes);

	_index = 0;
}

bool InitConditions_3D::AddValues(double x, double y, double z, double vx, double vy, double vz) {
	if (_index == _N)
		return EXIT_FAILURE;
	else {
		x0[_index]  = x;
		y0[_index]  = y;
		z0[_index]  = z;
		vx0[_index] = vx;
		vy0[_index] = vy;
		vz0[_index] = vz;

		_index++;
		return EXIT_SUCCESS;
	}
}

InitConditions_3D::~InitConditions_3D() {
	free(x0);
	free(y0);
	free(z0);
	free(vx0);
	free(vy0);
	free(vz0);
}

