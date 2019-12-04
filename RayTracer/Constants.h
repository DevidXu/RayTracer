#pragma once
#ifndef _CONSTANTS_H__
#define _CONSTANTS_H__

#define OMP_ACCR 1

#include "Vector.h"
#include "Helper.h"
#include "Model.h"
#include <memory>
#include <math.h>
using namespace std;

enum SPLIT_DIM { X_DIM, Y_DIM, Z_DIM, NONE_DIM };

class Patch;
class Model;
class KDTree;

typedef pair<shared_ptr<Patch>, float> HitInfo;
typedef vector<shared_ptr<Model>> ModelArr;
typedef vector<shared_ptr<Light>> LightArr;

class Constants:public Singleton<Constants> {
public:
	const int LEAF_TRIANGLE = 4;
	const int TRACE_DEPTH = 5;
	const float PI = 3.1415926f;
	const float epsilon = 0.00001f;
	const float attenuation = 0.95f;
	const Vector3 ENVIR_AMBIENT = Vector3(0.01f, 0.01f, 0.01f);
};

#endif