#pragma once
#ifndef _RAY_H__
#define _RAY_H__

#include "Vector.h"
#include <vector>
class Ray {
private:
	Vector3 position, direction;
	std::vector<Vector3> path;
	
	void transmit();
};

#endif