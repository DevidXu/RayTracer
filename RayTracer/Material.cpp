#include "Material.h"
#include "Constants.h"
#include "Ray.h"

Material::Material() {

}


Material::Material(float mEmiss, float mGlass, float mSpec, float mRough) {
	emissive = mEmiss;
	glass = mGlass;
	specular = mSpec;
}


float Material::getEmissive() {
	return emissive;
}

float Material::getGlass() {
	return glass;
}

float Material::getSpecular() {
	return specular;
}


void Material::setEmissive(float mEmiss) {
	emissive = mEmiss;
}

void Material::setGlass(float mGlass) {
	glass = mGlass;
	assert__(glass + specular <= 1.0f) {
		ERROR("Total glass color + specular color is larger than total color");
	}
}

void Material::setSpecular(float mSpec) {
	assert(mSpec <= 1.0f);
	specular = mSpec;
}


Vector3 Material::transmit(shared_ptr<Ray> ray, Vector3 normal) {
	const float epsilon = Constants::Instance()->epsilon;

	float possibility = (float)rand() / RAND_MAX;
	if (possibility > glass) return transmitDiffuse(ray, normal);

	float rayDotNor = ray->getDirection().dot(normal);
	if (abs(rayDotNor) < epsilon) return Vector3();
	if (abs(abs(rayDotNor) - 1.0f) < epsilon) return ray->getDirection();

	float sinIn = sqrt(1 - pow(rayDotNor, 2.0f));
	float tanIn = abs(sinIn / rayDotNor);
	float sinOut = rayDotNor < 0.0f ? sinIn / N : sinIn * N;
	if (sinOut >= 1.0f) return Vector3();
	float tanOut = abs(sinOut / sqrt(1 - pow(sinOut, 2.0f)));

	Vector3 outDir = normal * rayDotNor + (ray->getDirection() - normal * rayDotNor) * tanOut / tanIn;
	return outDir.normalize();
}


Vector3 Material::transmitDiffuse(shared_ptr<Ray> ray, Vector3 normal) {
	static float PI = Constants::Instance()->PI;
	static float epsilon = Constants::Instance()->epsilon;
	float possibility = (float)rand() / RAND_MAX;
	Vector3 dir = ray->getDirection();
	if (normal.dot(dir) > 0.0f) normal = Vector3() - normal;

	if (possibility * (1.0f - glass) < specular) {	// return specular light
		float len = -normal.dot(dir);
		return normal * len * 2 + dir;
	}
	// return diffuse light
	Vector3 reflected;
	int rand_limit = 5, count = 0;
	float sign = ray->getDirection().dot(normal) >= 0.0f ? -1.0f : 1.0f;
	while (reflected.dot(normal) * sign <= epsilon && count < rand_limit) {
		float angle1 = (float)rand() / RAND_MAX * PI * 2, angle2 = (float)rand() / RAND_MAX * PI;
		reflected = Vector3(sin(angle2)*cos(angle1), cos(angle2), sin(angle2)*sin(angle1));
		count += 1;
	}
	if (count == rand_limit) reflected = normal;
	return reflected;
}