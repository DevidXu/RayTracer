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
	vector<Vector3> positions, normals;
	vector<Vector2> texcoords;
	shared_ptr<Model> model;
public:
	shared_ptr<PatchArr> getPatches();
	shared_ptr<Model> getModel();
	void setModel(shared_ptr<Model> mModel);
};


class OBJShape : public Shape {
public:
	OBJShape(string filename);
};


class CubeShape : public Shape {
private:	// should use same structure as OBJ model
	Vector3 color;
public:
	CubeShape(vector<Vector3>& position, Vector3 color);
	CubeShape(Vector3 small, Vector3 large, Vector3 mColor);
	void generatePatches();
};


class SphereShape : public Shape {
private:
	Vector3 center, color;
public:
	SphereShape(Vector3 mCenter, Vector3 mColor, float mRadius);
};


class Model {
protected:
	shared_ptr<Shape> shape;
	shared_ptr<Material> material;
	string name;
public:
	Model(shared_ptr<Shape> mShape, shared_ptr<Material> mMaterial, string mName);
	
	shared_ptr<Material> getMaterial();
	shared_ptr<Shape> getShape();
	shared_ptr<PatchArr> getPatches();
	string getName();

	bool isLightSource();
};

class Light : public Model {
private:
	Vector3 center;
	float lightArea;
public:
	Light(shared_ptr<Shape> mShape, shared_ptr<Material> mMaterial, string mName);
	float getLightPDF(Vector3 point);
	Vector3 getCenter();
};
#endif