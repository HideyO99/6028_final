#pragma once
#include <glm/vec3.hpp>



class Vector3
{

public:

	Vector3();

	Vector3(const glm::vec3& v) : x(v.x), y(v.y), z(v.z) {}

	Vector3(float value);


	Vector3(float x, float y, float z);

	~Vector3();

	Vector3(const Vector3& rhs);

	const glm::vec3 GetGLM() const
	{
		return glm::vec3(x, y, z);
	}

	void Set(float newX, float newY, float newZ);


	Vector3& operator=(const Vector3& rhs);
	Vector3 operator+(const Vector3& rhs) const;
	Vector3 operator-(const Vector3& rhs) const;
	Vector3 operator-() const;
	Vector3 operator*(const float& scalar) const;
	Vector3 operator/(const float& scalar) const;
	void operator+=(const Vector3& rhs);
	void operator-=(const Vector3& rhs);
	void operator*=(const float& scalar);
	void operator/=(const float& scalar);


	void addScaledVector(const Vector3& vector, float scalar);
	void Normalize();
	float Magnitude() const;
	static float Distance(const Vector3& a, const Vector3& b);
	static float Length(const Vector3& v);
	Vector3 Inverse();

	float x;
	float y;
	float z;
};


