#pragma once
#include <string>
#include "..\MeshObj\cMeshObj.h"
#include <glm/glm.hpp>

#define STARTINGID 50

//struct sVector3
//{
//	sVector3() : x(0.f), y(0.f), z(0.f) {}
//	float x, y, z;
//};
class cGameObj
{
public:
	cGameObj();

	std::string name;
	float HP;
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 rotation;
	glm::vec3 direction;

	cMeshObj* pMeshObj;

	unsigned int getID();
	int cmdIndex;
private:
	unsigned int ID;
	static unsigned int nextID;
};

