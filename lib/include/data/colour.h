#pragma once

#include <iostream>
using namespace std;

class Colour {

public:
	float r;
	float g;
	float b;

	Colour() : r(0.0f), g(0.0f), b(0.0f) {}

	Colour(float mono) : r(mono), g(mono), b(mono) {}

	Colour(float red, float green, float blue) : r(red), g(green), b(blue) {}

	Colour& operator=(const Colour& rhs) {
		r = rhs.r;
		g = rhs.g;
		b = rhs.b;
		return *this;
	}

	// Note float arithmetic does not affect colour alpha.
	friend Colour operator+(const Colour &lhs, const Colour &rhs) {
		return Colour(lhs.r + rhs.r, lhs.g + rhs.g, lhs.b + rhs.b);
	}

	friend Colour operator-(const Colour &lhs, const Colour &rhs) {
		return Colour(lhs.r - rhs.r, lhs.g - rhs.g, lhs.b - rhs.b);
	}

	friend Colour operator*(const Colour &lhs, const float &rhs) {
		return Colour(lhs.r * rhs, lhs.g * rhs, lhs.b * rhs);
	}

	friend Colour operator*(const Colour &lhs, const Colour &rhs) {
		return Colour(lhs.r * rhs.r, lhs.g * rhs.g, lhs.b * rhs.b);
	}

	friend Colour operator/(const Colour &lhs, const float &rhs) {
		return Colour(lhs.r / rhs, lhs.g / rhs, lhs.b / rhs);
	}

	friend Colour operator/(const Colour &lhs, const Colour &rhs) {
		return Colour(lhs.r / rhs.r, lhs.g / rhs.g, lhs.b / rhs.b);
	}



	Colour& operator+=(const Colour &rhs) {
		this->add(rhs);
		return *this;
	}

	Colour& operator-=(const Colour &rhs) {
		this->sub(rhs);
		return *this;
	}

	Colour& operator*=(const float &rhs) {
		this->multf(rhs);
		return *this;
	}

	Colour& operator*=(const Colour &rhs) {
		this->mult(rhs);
		return *this;
	}

	Colour& operator/=(const float &rhs) {
		this->divf(rhs);
		return *this;
	}

	Colour& operator/=(const Colour &rhs) {
		this->div(rhs);
		return *this;
	}

	friend ostream& operator<<(ostream &os, const Colour &c) {
		os << "rgb{" << c.r << "," << c.g << "," << c.b<< "}"; 
		return os;
	}

private:
	// In place functions - only to be accessed through assignment operators.
	void mult(const Colour &scale) {
		r *= scale.r;
		g *= scale.g;
		b *= scale.b;
	}

	void multf(const float &scale) {
		r *= scale;
		g *= scale;
		b *= scale;
	}

	void div(const Colour &scale) {
		r /= scale.r;
		g /= scale.g;
		b /= scale.b;
	}

	void divf(const float &scale) {
		r /= scale;
		g /= scale;
		b /= scale;
	}

	void add(const Colour &other) {
		r += other.r;
		g += other.g;
		b += other.b;
	}

	void sub(const Colour &other) {
		r -= other.r;
		g -= other.g;
		b -= other.b;
	}
};
