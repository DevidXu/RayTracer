#pragma once
#ifndef _PATCH_H__
#define _PATCH_H__

#include "Vector.h"
#include "Ray.h"
#include <memory>

using namespace std;

struct Vertex {
	shared_ptr<Vector3> position;
	shared_ptr<Vector3> color;
	shared_ptr<Vector3> normal;
	shared_ptr<Vector2> texCoord;
	bool sample = false;
	Vertex();
	Vertex(Vector3& mPos);
	Vertex(Vector3& mPos, Vector3& mNor);
	Vertex(Vector3& mPos, Vector3& mNor, Vector3& mColor);
	Vertex(Vector3& mPos, Vector3& mNor, Vector2& mTex);
};

class Patch {
public:
	virtual bool intersect(shared_ptr<Ray> ray) = 0;
	
};


class Sphere : public Patch {
private:
	Vector3 center;

public:
	bool intersect(shared_ptr<Ray> ray);
};


class Triangle : public Patch {
public:
	shared_ptr<Vertex> points[3];
	Triangle(Vertex& v0, Vertex& v1, Vertex& v2);
private:
	bool intersect(shared_ptr<Ray> ray);
};



#endif