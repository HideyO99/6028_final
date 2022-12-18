#pragma once
#include <glm/glm.hpp>
#include "../MeshObj/cMeshObj.h"
#include "../VAOManager/cModelDrawInfo.h"

class cBoundingBox
{
public:
	glm::vec3 centerPointOffset;
	glm::vec3 minPointOffset;
	glm::vec3 maxPointOffset;
	glm::vec3 halfExtentOffset;

	glm::vec3 centerPoint;
	glm::vec3 minPoint;
	glm::vec3 maxPoint;
	glm::vec3 halfExtent;

	cMeshObj* pMeshObj;
	cModelDrawInfo* pDrawInfo;
};

