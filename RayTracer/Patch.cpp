#include "Patch.h"
#include "Ray.h"
#include <algorithm>

Vertex::Vertex() {
	sample = false;
}

Vertex::Vertex(shared_ptr<Vector3> mPos) {
	sample = false;
	position = mPos;
}

Vertex::Vertex(shared_ptr<Vector3> mPos, shared_ptr<Vector3> mNor) {
	sample = false;
	position = mPos;
	normal = mNor;
}



Vertex::Vertex(shared_ptr<Vector3> mPos, shared_ptr<Vector3> mNor, shared_ptr<Vector3> mColor) {
	sample = false;
	position = mPos;
	normal = mNor;
	color = mColor;
}

Vertex::Vertex(shared_ptr<Vector3> mPos, shared_ptr<Vector3> mNor, shared_ptr<Vector2> mTex) {
	sample = true;
	position = mPos;
	normal = mNor;
	texCoord = mTex;
}



Vector3 Patch::getCenter() {
	return center;
}

Vector3 Patch::getColor() {
	return color;
}

shared_ptr<Material> Patch::getMaterial() {
	return model->getMaterial();
}

shared_ptr<Model> Patch::getModel() {
	return model;
}

void Patch::setModel(shared_ptr<Model> mModel) {
	model = mModel;
}

string Patch::getModelName() {
	return model->getName();
}

Vector3 Patch::transmit(shared_ptr<Ray> ray, Vector3 hitPoint) {
	return model->getMaterial()->transmit(ray, hitPoint);
}


Sphere::Sphere(Vector3 mCenter, Vector3 mColor, float mRadius) {
	center = mCenter;
	color = mColor;
	radius = mRadius;
	assert__(radius > 0) {
		ERROR("Radius of sphere is not larger than 0");
	}
}


bool Sphere::crossPlane(float PX, float PY, float PZ, float PW) {
	assert__(PX*PX + PY * PY + PZ * PZ != 0.0) {
		ERROR("Invalid plane equation");
	}
	float sum_square = PX * PX + PY * PY + PZ * PZ;
	assert__(sum_square != 0.0) {
		ERROR("Invalid plane equation");
	}
	float dis = abs(center.dot(Vector3(PX, PY, PZ)) + PW) / sqrt(sum_square);
	return dis < radius;
}


shared_ptr<float> Sphere::intersect(shared_ptr<Ray> ray) {
	Vector3 x0 = center, x1 = ray->getPosition(), x2 = x1 + ray->getDirection();
	float numerator = ((x0 - x1).cross(x0 - x2)).magnitude();
	float denominator = (x2 - x1).magnitude();
	float distance = numerator / denominator;
	if (distance > radius) return nullptr;

	Vector3 pToC = center - x1;
	if (pToC.magnitude() >= radius) {
		if (pToC.dot(ray->getDirection()) <= 0.0f) return nullptr;
		distance = sqrt(pow(pToC.magnitude(), 2.0f) - distance * distance) - sqrt(radius *radius - distance * distance);
	} else {
		float sign = pToC.dot(ray->getDirection()) > 0.0f ? 1.0f : -1.0f;
		distance = sqrt(radius *radius - distance * distance) + sqrt(pow(pToC.magnitude(), 2.0f) - distance * distance) * sign;
	}
	return make_shared<float>(distance);
}

Vector3 Sphere::getNormal(Vector3 hitPoint) {
	return (hitPoint - center).normalize();
}

bool Sphere::fitInsideKDTree(Vector3 small, Vector3 large) {
	for (int i = 0; i < 3; i++) {
		float minVal = center.value[i] - radius;
		float maxVal = center.value[i] + radius;
		if (minVal < small.value[i]) return false;
		if (maxVal > large.value[i]) return false;
	}
	return true;
}


void Sphere::expandKDTree(Vector3& small, Vector3& large) {
	if (small == large) {
		small = center; large = center;
	}
	for (int i = 0; i < 3; i++) {
		small.value[i] = min(small.value[i], center.value[i] - radius);
		large.value[i] = max(large.value[i], center.value[i] + radius);
	}
}


float Sphere::getSurfaceArea() {
	float PI = Constants::Instance()->PI;
	return 4 * PI*radius*radius;
}


Triangle::Triangle(Vertex& v0, Vertex& v1, Vertex& v2) {
	// assert three distinct points
	assert__((*v0.position == *v1.position) == false && (*v0.position == *v2.position) == false && (*v1.position == *v2.position) == false) {
		ERROR("Same points in a triangle");
	}

	positions.push_back(v0.position);
	positions.push_back(v1.position);
	positions.push_back(v2.position);

	center = Vector3();
	assert__(v0.position != nullptr) {
		ERROR("Triangle does not contain position info");
	}
	center += *v0.position; center += *v1.position; center += *v2.position;
	center /= 3;

	color = Vector3();
	assert__(v0.normal != nullptr) {
		ERROR("Triangle does not contain normal info");
	}
	color += *v0.color; color += *v1.color; color += *v2.color;
	color /= 3;

	normal = Vector3(); Vector3 zero;
	normal += (v0.normal == nullptr ? zero : *v0.normal);
	normal += (v1.normal == nullptr ? zero : *v1.normal);
	normal += (v2.normal == nullptr ? zero : *v2.normal);
	if (normal.magnitude() == 0.0) {
		Vector3 AB = *v1.position - *v0.position, BC = *v2.position - *v1.position;
		normal = BC.cross(AB);
	}
	normal = normal.normalize();
}


bool Triangle::crossPlane(float PX, float PY, float PZ, float PW) {
	Vector3 planeVec(PX, PY, PZ);
	float v0 = positions[0]->dot(planeVec) + PW;
	float v1 = positions[1]->dot(planeVec) + PW;
	float v2 = positions[2]->dot(planeVec) + PW;
	if (v0 >= 0.0 && v1 >= 0.0 && v2 >= 0.0) return false;
	if (v0 <= 0.0 && v1 <= 0.0 && v2 <= 0.0) return false;
	return true;
}


bool Triangle::fitInsideKDTree(Vector3 small, Vector3 large) {
	assert(positions.size() == 3);
	for (int i = 0; i < 3; i++) {
		Vector3 point = *positions[i];
		for (int k = 0; k < 3; k++)
			if (point.value[k] < small.value[k] || point.value[k] > large.value[k]) 
				return false;
	}
	return true;
}


void Triangle::expandKDTree(Vector3& small, Vector3& large) {
	if (small == large) {
		small = center; large = center;
	}
	for (int i = 0; i < 3; i++) {
		for (int k = 0; k < 3; k++) {
			small.value[k] = min(small.value[k], positions[i]->value[k]);
			large.value[i] = max(large.value[k], positions[i]->value[k]);
		}
	}
}


shared_ptr<float> Triangle::intersect(shared_ptr<Ray> ray) {
	// judge whether intersection point is inside
	shared_ptr<Vector3> planePoint = ray->crossPlane(center, normal);
	if (planePoint == nullptr) return nullptr;
	assert(positions.size() == 3);
	Vector3 v0 = *positions[1] - *positions[0], v1 = *positions[2] - *positions[0], v2 = *planePoint - *positions[0];
	float dot00 = v0.dot(v0), dot01 = v0.dot(v1), dot02 = v0.dot(v2), dot11 = v1.dot(v1), dot12 = v1.dot(v2);
	
	float denominator = dot00 * dot11 - dot01 * dot01;
	assert__(denominator != 0.0f) {
		ERROR("180 degree in triangle leading to invalid intersection");
	}
	float u = (dot11 * dot02 - dot01 * dot12) / denominator;
	float v = (dot00 * dot12 - dot01 * dot02) / denominator;
	if (u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f || u + v > 1.0f) return nullptr;

	float distance = (*planePoint - ray->getPosition()).magnitude();
	return make_shared<float>(distance);
}


Vector3 Triangle::getNormal(Vector3 hitPoint) {
	return normal;
}


float Triangle::getSurfaceArea() {
	Vector3 edge1 = *positions[0] - *positions[1], edge2 = *positions[0] - *positions[2];
	return edge1.cross(edge2).magnitude()*0.5f;
}