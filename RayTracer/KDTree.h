#pragma once
#ifndef _KDTREE_H__
#define _KDTREE_H__

#include "Model.h"
#include "Vector.h"
#include "Patch.h"
#include "Constants.h"
#include "Logger.h"
#include "Ray.h"
#include <unordered_map>
#include <memory>

using namespace std;

class KDTree {
private:
	SPLIT_DIM dim;
	float midVal = 0.0;
	Vector3 small, large;
	PatchArr patches;
	shared_ptr<KDTree> leftTree, rightTree;

	// decide which dimension to split & calculate midVal
	SPLIT_DIM chooseSplitDim(vector<Vector3>& centers);
public:
	KDTree(Vector3 mSmall, Vector3 mLarge, SPLIT_DIM init_dim);

	// collect patches from all models and put into KD tree
	void constructFromPatches(PatchArr& mPatches);

	// build sub KD tree by filtering local patches and generate leftTree & rightTree;
	// this forms recursive call with constructFromPatches
	void buildSubKDTree(PatchArr& mPatches);

	int getDepth();
	int getLocalSize();
	int getTotalSize();
	
	// test whether patch can fit in this layer (instead of passing down)
	bool exactFitPatch(shared_ptr<Patch> patch, float midVal);

	// judge whether a point is on the boundary of dim dimension
	bool onBoundary(Vector3 point, int dim);

	// test whether patch can fit inside the whole space
	bool fullFitPatch(shared_ptr<Patch> patch);

	// whether the KD tree is met by ray
	bool hitRay(shared_ptr<Ray> ray, float distance);
	// whether there is any patch in this layer's KD tree intersect with ray
	void intersectLocal(shared_ptr<Ray> ray, HitInfo& info);

	// whether there is a patch hit by ray (all sub KD trees);
	// distance is current minimum distance; update it if there exists smaller distance
	void intersectWithRay(shared_ptr<Ray> ray, HitInfo& hitInfo);
};

#endif
