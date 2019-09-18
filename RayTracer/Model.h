#pragma once
#ifndef _MODEL_H__
#define _MODEL_H__

#include <string>
#include <vector>
#include "Patch.h"
#include "Material.h"

using namespace std;

typedef vector<shared_ptr<Patch>> PatchArr;
class Shape {
protected:
	PatchArr patches;
	vector<Vertex> vertexes;
	vector<Vector3> positions, normals;
	vector<Vector2> texcoords;
public:
	shared_ptr<PatchArr> getPatches();
};


class OBJShape : public Shape {
private:
	void generateVertexes(vector<Vector3>& positions, vector<Vector3>& normals, vector<Vector2>& texcoords);
public:
	void loadOBJShape(std::string filename);
};


class CubeShape : public Shape {
private:	// should use same structure as OBJ model
public:
	void loadCubeShape(vector<Vector3>& position, Vector3 color);
};


class SphereShape : public Shape {
private:
	Vector3 center, color;
public:
	void loadSphereShape(Vector3& mCenter, Vector3& mColor);
};


class Model {
private:
	shared_ptr<Shape> shape;
	Material material;
public:
	void setShape(shared_ptr<Shape> mShape);
	shared_ptr<vector<Patch>> getPatches();
};
#endif