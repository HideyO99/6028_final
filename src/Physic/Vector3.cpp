#include "Vector3.h"
#include <cmath>

Vector3::Vector3()
	: Vector3(0.f)
{

}

Vector3::Vector3(float value)
	: Vector3(value, value, value)
{

}


Vector3::Vector3(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}


Vector3::~Vector3() {

}


Vector3::Vector3(const Vector3& rhs)

{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
}


void Vector3::Set(float newX, float newY, float newZ)
{
	x = newX;
	y = newY;
	z = newZ;
}


Vector3& Vector3::operator=(const Vector3& rhs) {
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	return *this;
}


Vector3 Vector3::operator+(const Vector3& rhs) const {
	return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
}


Vector3 Vector3::operator-(const Vector3& rhs) const {
	return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
}


Vector3 Vector3::operator-() const {
	return Vector3(-x, -y, -z);
}


Vector3 Vector3::operator*(const float& scalar) const {
	return Vector3(x * scalar, y * scalar, z * scalar);
}


Vector3 Vector3::operator/(const float& scalar) const {
	return Vector3(x / scalar, y / scalar, z / scalar);
}


void Vector3::operator+=(const Vector3& rhs) {
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
}


void Vector3::operator-=(const Vector3& rhs) {
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
}


void Vector3::operator*=(const float& scalar) {
	x *= scalar;
	y *= scalar;
	z *= scalar;
}


void Vector3::operator/=(const float& scalar) {
	x /= scalar;
	y /= scalar;
	z /= scalar;
}


void Vector3::addScaledVector(const Vector3& vector, float scalar)
{
	*this += vector * scalar;
}


void Vector3::Normalize() {
	float m = Magnitude();
	x /= m;
	y /= m;
	z /= m;
}


float Vector3::Magnitude() const {
	return sqrt(x * x + y * y + z * z);
}

Vector3 Vector3::Inverse() 
{
	return Vector3(x, y, z) * -1.f;
}


float Vector3::Length(const Vector3& v)
{
	return v.Magnitude();
}


float Vector3::Distance(const Vector3& a, const Vector3& b)
{
	return (a - b).Magnitude();
}

