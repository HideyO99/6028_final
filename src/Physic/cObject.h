#pragma once
#include "cBoundingBox.h"
#include "../MeshObj/cMeshObj.h"
#include "shape.h"


class cObject
{
public:
	cObject();
	~cObject();

	void setMass(float mass);
	void integrate(float dT);
	void applyForce(const glm::vec3& f_dir);
	void killAllForces();
	void update();

	cMeshObj* pMeshObj;
	cShape* pShape;
	cBoundingBox* pBBox;
	std::string objName;
	glm::vec3 position;
	glm::vec3 prevPosition;
	glm::vec3 direction;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 force;
	float damping;
	float invMass;
	bool isHover;
};

