#include "Vector.h"
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <cassert>

std::ostream &operator<<(std::ostream& out, const Vector2 v) {
	if (&v == nullptr) {
		std::cout << "None ";
		return out;
	}
	out << "[" << std::setprecision(3) << v.value[0] << ", " << v.value[1] << "] ";
	return out;
}

std::ostream &operator<<(std::ostream& out, const Vector3 v) {
	if (&v == nullptr) {
		std::cout << "None ";
		return out;
	}
	out << "[" << std::setprecision(3) << v.value[0] << ", " << v.value[1] << ", " << v.value[2] << "] ";
	return out;
}

Vector2::Vector2() {
	value[0] = value[1] = 0.0;
}

Vector2::Vector2(float x0, float x1) {
	value[0] = x0;
	value[1] = x1;
}

Vector2::Vector2(const Vector2& v) {
	for (int i = 0; i < 2; i++) value[i] = v.value[i];
}


Vector3::Vector3() {
	value[0] = value[1] = value[2] = 0.0;
}

Vector3::Vector3(const Vector3& v) {
	for (int i = 0; i < 3; i++) value[i] = v.value[i];
}

Vector3::Vector3(float x0, float x1, float x2) {
	value[0] = x0;
	value[1] = x1;
	value[2] = x2;
}

bool Vector3::operator==(const Vector3 &v) {
	bool result = true;
	for (int i = 0; i < 3; i++) result &= (value[i] == v.value[i]);
	return result;
}

Vector3 Vector3::operator+(const Vector3 &v) {
	Vector3 result;
	for (int i = 0; i < 3; i++) result.value[i] = value[i] + v.value[i];
	return result;
}

void Vector3::operator+=(const Vector3 &v) {
	for (int i = 0; i < 3; i++) value[i] += v.value[i];
}

Vector3 Vector3::operator-(const Vector3 &v) {
	Vector3 result;
	for (int i = 0; i < 3; i++) result.value[i] = value[i] - v.value[i];
	return result;
}

void Vector3::operator-=(const Vector3 &v) {
	for (int i = 0; i < 3; i++) value[i] -= v.value[i];
}

void Vector3::operator=(const Vector3 &v) {
	for (int i = 0; i < 3; i++) value[i] = v.value[i];
}

float Vector3::dot(const Vector3 &v) {
	float result = 0.0;
	for (int i = 0; i < 3; i++) result += value[i] * v.value[i];
	return result;
}

float Vector3::magnitude() {
	float result = 0.0;
	for (int i = 0; i < 3; i++) result += value[i] * value[i];
	return sqrt(result);
}

float Vector3::distance(const Vector3 &v) {
	float result = 0.0;
	for (int i = 0; i < 3; i++) result += (value[i] - v.value[i]) * (value[i] - v.value[i]);
	return sqrt(result);
}

Vector3 Vector3::normalize() {
	float length = magnitude();
	assert__(length > 0.0) {
		ERROR("Vector of len 0 cannot be normalized");
	}
	return *this * (1.0f / length);
}

Vector3 Vector3::operator*(float k) {
	Vector3 result;
	for (int i = 0; i < 3; i++) result.value[i] = value[i] * k;
	return result;
}

Vector3 Vector3::operator*(const Vector3 &v) {
	Vector3 result;
	for (int i = 0; i < 3; i++) result.value[i] = value[i] * v.value[i];
	return result;
}

void Vector3::operator*=(float k) {
	for (int i = 0; i < 3; i++) value[i] *= k;
}

Vector3 Vector3::operator/(float k) {
	assert__(k != 0.0) {
		ERROR("Invalid Vector operation: divide by 0");
	}
	Vector3 result;
	for (int i = 0; i < 3; i++) result.value[i] = value[i] / k;
	return result;
}

void Vector3::operator/=(float k) {
	assert__(k != 0.0) {
		ERROR("Invalid Vector operation: divide by 0");
	}
	for (int i = 0; i < 3; i++) value[i] /= k;
}


Vector3 Vector3::cross(const Vector3 &v) {
	float v0 = value[1] * v.value[2] - value[2] * v.value[1];
	float v1 = -(value[0] * v.value[2] - value[2] * v.value[0]);
	float v2 = value[0] * v.value[1] - value[1] * v.value[0];
	return Vector3(v0, v1, v2);
}


string Vector3::toString() {
	return "(" + to_string(value[0]) + ", " + to_string(value[1]) + ", " + to_string(value[2]) + ") ";
}