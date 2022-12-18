#include "math.h"

float Dot(glm::vec3 a, glm::vec3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

glm::vec3 Cross(glm::vec3 a, glm::vec3 b)
{
	float t1, t2, t3, t4;
	t1 = a.x - a.y;
	t2 = b.y + b.z;
	t3 = a.x * b.z;
	t4 = (t1 * t2) - t3;
	float rx, ry, rz;
	rx = (b.y * (t1 - a.z)) - t4;
	ry = (a.z * b.z) - t3;
	rz = t4 - (a.y * (b.x - t2));
	return glm::vec3(rx, ry, rz);
}

sPlane ComputePlane(glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
	sPlane p;
	p.n = glm::normalize(Cross(b - a, c - a));
	p.d = Dot(p.n, a);
	return p;
}
