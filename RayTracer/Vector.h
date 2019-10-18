#pragma once
#ifndef _VECTOR_H__
#define _VECTOR_H__

#include "Logger.h"
#include <ostream>

struct Vector2 {
	float value[2];
	Vector2();
	Vector2(float x0, float x1);
	Vector2(const Vector2& v);
};

struct Vector3 {
	float value[3];
	Vector3();
	Vector3(const Vector3& v);
	Vector3(float x0, float x1, float x2);

	bool operator==(const Vector3 &v);
	Vector3 operator+(const Vector3 &v);
	void operator+=(const Vector3 &v);
	Vector3 operator-(const Vector3 &v);
	void operator-=(const Vector3 &v);
	void operator=(const Vector3 &v);
	Vector3 operator*(float k);
	Vector3 operator*(const Vector3 &v);
	void operator*=(float k);
	Vector3 operator/(float k);
	void operator/=(float k);
	Vector3 cross(const Vector3 &v);

	float dot(const Vector3 &v);
	float magnitude();
	float distance(const Vector3 &v);
	Vector3 normalize();
	string toString();
};

std::ostream &operator<<(std::ostream& out, const Vector3 v);

#endif