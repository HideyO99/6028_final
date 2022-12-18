#pragma once
#include <glm/glm.hpp>


float Dot(glm::vec3 a, glm::vec3 b);
glm::vec3 Cross(glm::vec3 a, glm::vec3 b);

struct sPlane
{
	glm::vec3 n;
	float d;
};

sPlane ComputePlane(glm::vec3 a, glm::vec3 b, glm::vec3 c);