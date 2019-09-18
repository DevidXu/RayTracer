#pragma once
#ifndef _CONSTANTS_H__
#define _CONSTANTS_H__

#define WIDTH 500
#define HEIGHT 500
#define SAMPLE_PER_PIXEL 10

#include "Vector.h"
#include "Helper.h"
#include "Model.h"
#include "KDTree.h"
#include <memory>
using namespace std;

typedef vector<shared_ptr<Model>> ModelArr;
class Constants:public Singleton<Constants> {
public:
	int height, width;
	int xStart, yStart, xEnd, yEnd;
	Vector3 cameraPosition, cameraDirection;
	shared_ptr<ModelArr> models;
	shared_ptr<KDTree> kdTree;
public:
	Constants() {
		models = make_shared<ModelArr>(ModelArr());
		kdTree = make_shared<KDTree>(new KDTree());
	}
public:
	void addModel(shared_ptr<Model> modelPtr) { models->push_back(modelPtr); }
	shared_ptr<ModelArr> getModels() { return models; }
	shared_ptr<KDTree> getKDTree() { return kdTree; }
};

#endif