#pragma once
#ifndef _RAY_H__
#define _RAY_H__

#include "Vector.h"
#include "Constants.h"
#include <vector>

using namespace std;

class Ray {
private:
	Vector3 position, direction;
	std::vector<Vector3> path;
	
	void transmit();
public:
	Ray(Vector3 position, Vector3 direction);

	// point-normal style plane
	shared_ptr<Vector3> crossPlane(Vector3 point, Vector3 normal);
	Vector3 getPosition();
	Vector3 getDirection();
	void setDirection(Vector3 dir);
	void setPosition(Vector3 pos);
	void updateTrace(Vector3 pos, Vector3 dir);

	void march(float distance);
};

typedef vector<shared_ptr<Ray>> Rays;

#endif