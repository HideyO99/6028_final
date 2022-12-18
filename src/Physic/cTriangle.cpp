#include "cTriangle.h"

cTriangle::cTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
	pointA = a;
	pointB = b;
	pointC = c;
}

cTriangle::~cTriangle()
{
}