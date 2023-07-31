#pragma once

#include <math.h>
#include <iostream>

class Vector {
public:
	float x;
	float y;
	float z;

	Vector() : x(0.0f), y(0.0f), z(0.0f) {}

	Vector(float v) : x(v), y(v), z(v) {}

	Vector(float x, float y, float z) : x(x), y(y), z(z) {}

	static Vector zeroes() {
		return Vector(0.0f);
	}

	static bool is_zero(const Vector &v) {
		// TODO: Redundant once equality operator implemented.
		return v.x == 0.0f && v.y == 0.0f && v.z == 0.0f;
	}

	void negate() {
		x = -x;
		y = -y;
		z = -z;
	}

	float length_squared() {
	  return (x * x + y * y + z * z);
	}

	float length() {
	  return sqrtf(x * x + y * y + z * z);
	}

	void normalise() {
		float mag = length();
		if (mag == 0.0f) return;
		x = x / mag;
		y = y / mag;
		z = z / mag;
	}

	static Vector v_abs(Vector v) {
		return Vector(
			fabs(v.x),
			fabs(v.y),
			fabs(v.z)
		);
	}

	static Vector v_max(Vector a, Vector b) {
		return Vector(
			std::max(a.x, b.x),
			std::max(a.y, b.y),
			std::max(a.z, b.z)
		);
	}

	static Vector v_min(Vector a, Vector b) {
		return Vector(
			std::min(a.x, b.x),
			std::min(a.y, b.y),
			std::min(a.z, b.z)
		);
	}

	static Vector v_lerp(Vector a, Vector b, Vector r) {
		return r * a + (1 - r) * b;
	}

	static Vector norm(Vector v) {
		float mag = v.length();
		if (mag == 0.0f) return Vector(0.0f, 0.0f, 0.0f);
		return Vector(v.x / mag, v.y / mag, v.z / mag);
	}

	float dot(Vector o) {
		return x * o.x + y * o.y + z * o.z;
	}

	static float dot_product(Vector a, Vector b) {
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	void cross(Vector &other, Vector &result) {
		result.x = y * other.z - z * other.y;
		result.y = z * other.x - x * other.z;
		result.z = x * other.y - y * other.x;
	}

	void cross(Vector &other) {
		Vector result;
		result.x = y * other.z - z * other.y;
		result.y = z * other.x - x * other.z;
		result.z = x * other.y - y * other.x;

		x = result.x;
		y = result.y;
		z = result.z;
	}

	static Vector cross_product(Vector a, Vector b) {
		Vector result;
		result.x = a.y * b.z - a.z * b.y;
		result.y = a.z * b.x - a.x * b.z;
		result.z = a.x * b.y - a.y * b.x;

		return result;
	}

	static Vector reflect(Vector wi, Vector n) {
		return wi - 2.0f * n.dot(wi) * n;
	}

	// bool operator==(const Vector &a, const Vector &b) {
	// 	return typeid(a) == typeid(b) && a.x == b.x && a.y = b.y && a.z == b.z;
	// }

	float operator[](int i) const {
		if (i == 0) return x;
		if (i == 1) return y;
		if (i == 2) return z;
		throw std::logic_error("ERR: out of bounds access of vector: Vector["+ std::to_string(i) + "]");
	}
	
	float& operator[](int i) {
		if (i == 0) return x;
		if (i == 1) return y;
		if (i == 2) return z;
		throw std::logic_error("ERR: out of bounds access of vector: Vector["+ std::to_string(i) + "]");
	}

	Vector& operator=(Vector other) {
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}

	friend Vector operator+(const Vector &a, const Vector &b) {
		return Vector(a.x + b.x, a.y + b.y, a.z + b.z);
	}

	Vector& operator+=(const Vector &rhs) {
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	friend Vector operator-(const Vector &a, const Vector &b) {
		return Vector(a.x - b.x, a.y - b.y, a.z - b.z);
	}

	Vector& operator-=(const Vector &rhs) {
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}

	friend Vector operator-(const Vector &a) {
		return -1.0f * a;
	}

	friend Vector operator*(const Vector &a, const Vector &b) {
		return Vector(a.x * b.x, a.y * b.y, a.z * b.z);
	}

	friend Vector operator*(const float a, const Vector &b) {
		return Vector(a * b.x, a * b.y, a * b.z);
	}

	Vector& operator*=(const float &rhs) {
		x *= rhs;
		y *= rhs;
		z *= rhs;
		return *this;
	}


	friend Vector operator/(const Vector &a, const float b) {
		return Vector(a.x / b, a.y / b, a.z / b);
	}

	friend Vector operator/(const float a, const Vector &b) {
		return Vector(a / b.x, a / b.y, a / b.z);
	}

	Vector& operator/=(const float &rhs) {
		x /= rhs;
		y /= rhs;
		z /= rhs;
		return *this;
	}

	friend std::ostream& operator<<(std::ostream &os, const Vector &v) {
		os << "Vector{" << v.x << "," << v.y << "," << v.z<< "}"; 
		return os;
	}

};
