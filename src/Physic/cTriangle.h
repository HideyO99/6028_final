#pragma once
#include <glm/glm.hpp>

class cTriangle
{
public:
	cTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c);
	~cTriangle();

	glm::vec3 pointA;
	glm::vec3 pointB;
	glm::vec3 pointC;

private:

};

