#include "Ray.h"


Ray::Ray(Vector3 mPosition, Vector3 mDirection) {
	position = mPosition;
	direction = mDirection.normalize();
}

Vector3 Ray::getDirection() {
	return direction;
}

Vector3 Ray::getPosition() {
	return position;
}

void Ray::setDirection(Vector3 dir) {
	direction = dir;
}

void Ray::setPosition(Vector3 pos) {
	position = pos;
}

void Ray::updateTrace(Vector3 pos, Vector3 dir) {
	position = pos;
	direction = dir;
}

void Ray::march(float distance) {
	position += direction * distance;
}

shared_ptr<Vector3> Ray::crossPlane(Vector3 point, Vector3 normal) {
	float deminator = 0.0f, numerator = 0.0f;
	for (int i = 0; i < 3; i++) {
		deminator += normal.value[i] * direction.value[i];
		numerator += (point.value[i] - position.value[i]) *normal.value[i];
	}

	float epsilon = Constants::Instance()->epsilon;
	if (abs(deminator) < epsilon) return nullptr;
	float distance = numerator / deminator;
	if (distance < 0.0f) return nullptr;
	return make_shared<Vector3>(position + direction * distance);
}