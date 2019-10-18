#pragma once
#ifndef _RASTER_H__
#define _RASTER_H__

#include "Singleton.h"
#include "Logger.h"
#include "Ray.h"
#include "KDTree.h"

#define MAX_COLOR 255

typedef vector<vector<Vector3>> Picture;
typedef vector<pair<string, float>> HitHistory;

class Raster:public Singleton<Raster> {
private:
	Rays emitRays(float angleX, float angleY, int num);
	Vector3 traceRay(shared_ptr<Ray> ray, const shared_ptr<KDTree>& kdTree, shared_ptr<HitHistory> hitHistory);
	void debugSinglePixel(int xPixel, int yPixel);
	Vector3 directLight(shared_ptr<Ray> ray, shared_ptr<Patch> hitPatch, Vector3 hitPoint);

public:
	int height, width;
	int xStart, yStart, xEnd, yEnd;
	int sampleNum;
	float angleHeight, angleWidth;
	Vector3 cameraPosition, cameraDirection;

	shared_ptr<ModelArr> models;
	shared_ptr<LightArr> lights;
	Picture picture;
	shared_ptr<KDTree> kdTree;

	Raster();
	void render();
	void printParameters();

	void addModel(shared_ptr<Model> modelPtr);
	shared_ptr<ModelArr> getModels();
	void addLight(shared_ptr<Light> light);
	shared_ptr<LightArr> getLights();
	shared_ptr<KDTree> getKDTree();

	void normalize();
	void writeToPPM(string fileName);
};

#endif