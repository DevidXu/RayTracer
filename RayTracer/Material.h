#pragma once
#ifndef _MATERIAL_H__
#define _MATERIAL_H__

#include "Vector.h"
#include <memory>

using namespace std;

class Ray;

class Material {
public:
	Material();
	Material(float mEmiss, float mGlass, float mSpec, float mRough);
	float getGlass();
	float getEmissive();
	float getSpecular();
	void setGlass(float mGlass);
	void setEmissive(float mEmiss);
	void setSpecular(float mSpec);
	Vector3 transmit(shared_ptr<Ray> ray, Vector3 normal);

private:
	float emissive = 0.0f;
	float specular = 0.0f;
	float glass = 0.0f, N = 1.55f;	// refractive index

	Vector3 transmitDiffuse(shared_ptr<Ray> ray, Vector3 normal);
};

#endif