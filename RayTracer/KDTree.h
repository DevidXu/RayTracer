#pragma once
#ifndef _KDTREE_H__
#define _KDTREE_H__

#include "Model.h"

using namespace std;

class KDTree {
protected:
	void split();
public:
	// add patches of model into KDtree; used during initialization
	void addModelPatches(shared_ptr<Model> modelPtr);
	void intersectWithRay();
};

#endif
