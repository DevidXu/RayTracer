#include "KDTree.h"
#include <algorithm>
#include <numeric>

using namespace std;

const unordered_map<SPLIT_DIM, int> DIM_Map = { { X_DIM, 0 },{ Y_DIM, 1 },{ Z_DIM, 2 },{ NONE_DIM, -1 } };

KDTree::KDTree(Vector3 mSmall, Vector3 mLarge, SPLIT_DIM init_dim) {
	Vector3 expand(0.1f, 0.1f, 0.1f);
	small = mSmall - expand; large = mLarge + expand;
	dim = init_dim;
}


bool KDTree::exactFitPatch(shared_ptr<Patch> patch, float midVal) {
	// plane function: PX V0 + PY V1 + PZ V2 + PW = 0;
	float PX = float(dim == X_DIM ? 1.0 : 0.0);
	float PY = float(dim == Y_DIM ? 1.0 : 0.0);
	float PZ = float(dim == Z_DIM ? 1.0 : 0.0);
	float PW = -midVal;
	return patch->crossPlane(PX, PY, PZ, PW);
}


bool KDTree::fullFitPatch(shared_ptr<Patch> patch) {
	return patch->fitInsideKDTree(small, large);
}


float getVariance(vector<float>& v, float& mean) {
	assert__(v.size() > 1) {
		ERROR("Try calculating mean/variance of array with length = 0");
	}
	float sum = accumulate(begin(v), end(v), 0.0f);
	mean = sum / (int)v.size();

	float accum = 0.0;
	for_each(begin(v), end(v), [&](const float d) {
		accum += (d - mean)*(d - mean);
	});

	return sqrt(accum / ((int)v.size() - 1));
}


SPLIT_DIM KDTree::chooseSplitDim(vector<Vector3>& centers) {
	vector<float> means(3), variances(3);
	for (int i = 0; i < 3; i++) {
		vector<float> nums;
		for (auto & point : centers) nums.push_back(point.value[i]);
		float variance = getVariance(nums, means[i]);
		variances[i] = variance;
	}
	assert(variances.size() == 3);
	assert(means.size() == 3);
	int idx = 0;
	float maxVariance = variances[0];
	midVal = means[0];
	for (int i = 0; i < 3; i++) {
		if (variances[i] > maxVariance) {
			maxVariance = variances[i];
			idx = i;
			midVal = means[i];
		}
	}
	switch (idx) {
	case 0: return X_DIM; break;
	case 1: return Y_DIM; break;
	case 2: return Z_DIM; break;
	default: assert(false); break;
	}
	return NONE_DIM;
}


void KDTree::constructFromPatches(PatchArr& mPatches) {
	// if there are few triangles, stop splitting the tree
	if ((int)mPatches.size() <= Constants::Instance()->LEAF_TRIANGLE) {
		for (auto & patch : mPatches) {
			patches.push_back(patch);
		}
		dim = NONE_DIM;
		return;
	}

	vector<Vector3> centers;
	for (auto & patch : mPatches) centers.push_back(patch->getCenter());
	
	Vector3 left_Large(large), right_Small(small);
	dim = chooseSplitDim(centers);

	switch (dim) {
		case X_DIM: 
			left_Large.value[0] = midVal;
			right_Small.value[0] = midVal;
			break;
		case Y_DIM: 
			left_Large.value[1] = midVal;
			right_Small.value[1] = midVal;
			break;
		case Z_DIM: 
			left_Large.value[2] = midVal;
			right_Small.value[2] = midVal;
			break;
		default: assert__(false) { ERROR("Current layer has NONE dim when splitting"); }
			break;
	}
	
	leftTree = make_shared<KDTree>(small, left_Large, NONE_DIM);
	rightTree = make_shared<KDTree>(right_Small, large, NONE_DIM);

	buildSubKDTree(mPatches);
}


void KDTree::buildSubKDTree(PatchArr& mPatches) {
	PatchArr leftPatches, rightPatches;
	for (auto & patch : mPatches) {
		if (exactFitPatch(patch, midVal)) patches.push_back(patch);
		else {
			Vector3 center = patch->getCenter();
			float centerVal = 0.0;
			switch (dim) {
			case X_DIM: centerVal = center.value[0]; break;
			case Y_DIM: centerVal = center.value[1]; break;
			case Z_DIM: centerVal = center.value[2]; break;
			default: assert(false); break;
			}
			if (centerVal <= midVal) leftPatches.push_back(patch);
			else rightPatches.push_back(patch);
		}
	}
	string sizeInfo = "local size : " + to_string(patches.size()) + "; left size : " + to_string(leftPatches.size()) + "; right size : " + to_string(rightPatches.size());
	INFO("Construct sub Tree; " + sizeInfo);
	leftTree->constructFromPatches(leftPatches);
	rightTree->constructFromPatches(rightPatches);
}


int KDTree::getDepth() {
	if (!leftTree && !rightTree) return 1;
	return max(leftTree->getDepth(), rightTree->getDepth()) + 1;
}


int KDTree::getLocalSize() {
	return patches.size();
}


int KDTree::getTotalSize() {
	int totalSize = getLocalSize();
	if (leftTree) totalSize += leftTree->getTotalSize();
	if (rightTree) totalSize += rightTree->getTotalSize();
	return totalSize;
}


void KDTree::intersectLocal(shared_ptr<Ray> ray, HitInfo& hitInfo) {
	shared_ptr<Patch> hitPatch;
	for (auto & patch : patches) {
		shared_ptr<float> hit = patch->intersect(ray);
		if (hit != nullptr && *hit < hitInfo.second) {
			hitInfo = HitInfo(patch, *hit);
		}
	}
	return;
}


bool KDTree::onBoundary(Vector3 point, int dim) {
	bool fitInside = true;
	const float epsilon = Constants::Instance()->epsilon;
	if (abs(point.value[dim] - small.value[dim]) > epsilon && abs(point.value[dim] - large.value[dim]) > epsilon) 
		return false;

	for (int k = 0; k < 3; k++)
		if (k != dim && (point.value[k]<small.value[k] - epsilon || point.value[k]>large.value[k] + epsilon))
			return false;
	return true;
}


bool KDTree::hitRay(shared_ptr<Ray> ray, float distance) {
	Vector3 rayPos = ray->getPosition();
	bool fitInside = true;
	static float epsilon = Constants::Instance()->epsilon;
	for (int i = 0; i < 3; i++) 
		if (rayPos.value[i] < small.value[i] - epsilon || rayPos.value[i] > large.value[i] + epsilon) {
			fitInside = false;
			break;
		}
	if (fitInside) return true;

	for (int i = 0; i < 3; i++) {
		Vector3 normal;
		normal.value[i] = -1.0f;
		shared_ptr<Vector3> hitPoint = ray->crossPlane(small, normal);
		if (hitPoint != nullptr && onBoundary(*hitPoint, i)) return true;
		normal.value[i] = 1.0f;
		hitPoint = ray->crossPlane(large, normal);
		if (hitPoint != nullptr && onBoundary(*hitPoint, i)) return true;
	}

	return false;
}


void KDTree::intersectWithRay(shared_ptr<Ray> ray, HitInfo& hitInfo) {
	if (!hitRay(ray, hitInfo.second)) return;
	
	intersectLocal(ray, hitInfo);
	if (leftTree == nullptr && rightTree == nullptr) return;

	Vector3 rayPos = ray->getPosition();
	int splitDim = -1;
	switch (dim) {
	case X_DIM: splitDim = 0; break;
	case Y_DIM: splitDim = 1; break;
	case Z_DIM: splitDim = 2; break;
	default: assert(false); break;
	}
	if (rayPos.value[splitDim] <= midVal) {
		leftTree->intersectWithRay(ray, hitInfo);
		rightTree->intersectWithRay(ray, hitInfo);
	} else {
		rightTree->intersectWithRay(ray, hitInfo);
		leftTree->intersectWithRay(ray, hitInfo);
	}
}