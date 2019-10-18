#pragma once
#ifndef _PATCH_H__
#define _PATCH_H__

#include "Vector.h"
#include "Logger.h"
#include "Material.h"
#include <assert.h>
#include <memory>
#include <vector>

using namespace std;

class Model;

struct Vertex {
	shared_ptr<Vector3> position;
	shared_ptr<Vector3> color;
	shared_ptr<Vector3> normal;
	shared_ptr<Vector2> texCoord;
	bool sample = false;
	Vertex();
	Vertex(shared_ptr<Vector3> mPos);
	Vertex(shared_ptr<Vector3> mPos, shared_ptr<Vector3> mNor);
	Vertex(shared_ptr<Vector3> mPos, shared_ptr<Vector3> mNor, shared_ptr<Vector3> mColor);
	Vertex(shared_ptr<Vector3> mPos, shared_ptr<Vector3> mNor, shared_ptr<Vector2> mTex);
};


class Ray;

class Patch {
public:
	virtual shared_ptr<float> intersect(shared_ptr<Ray> ray) = 0;
	virtual bool crossPlane(float PX, float PY, float PZ, float PW) = 0;
	virtual bool fitInsideKDTree(Vector3 small, Vector3 large) = 0;
	virtual void expandKDTree(Vector3& small, Vector3& large) = 0;
	virtual Vector3 getNormal(Vector3 hitPoint) = 0;
	virtual float getSurfaceArea() = 0;
	Vector3 transmit(shared_ptr<Ray> ray, Vector3 hitPoint);
	Vector3 getColor();
	Vector3 getCenter();
	string getModelName();
	shared_ptr<Material> getMaterial();
	shared_ptr<Model> getModel();
	void setModel(shared_ptr<Model> mModel);
protected:
	Vector3 color, center;
	shared_ptr<Model> model;

};


class Sphere : public Patch {
private:
	float radius;
public:
	Sphere(Vector3 mCenter, Vector3 mColor, float mRadius);
	shared_ptr<float> intersect(shared_ptr<Ray> ray);
	bool crossPlane(float PX, float PY, float PZ, float PW);
	bool fitInsideKDTree(Vector3 small, Vector3 large);
	void expandKDTree(Vector3& small, Vector3& large);
	float getSurfaceArea();
	Vector3 getNormal(Vector3 hitPoint);
};


class Triangle : public Patch {
public:
	Triangle(Vertex& v0, Vertex& v1, Vertex& v2);
	shared_ptr<float> intersect(shared_ptr<Ray> ray);
	bool crossPlane(float PX, float PY, float PZ, float PW);
	bool fitInsideKDTree(Vector3 small, Vector3 large);
	void expandKDTree(Vector3& small, Vector3& large);
	float getSurfaceArea();
	Vector3 getNormal(Vector3 hitPoint);
private:
	vector<shared_ptr<Vector3>> positions;
	Vector3 normal;
};

#endif