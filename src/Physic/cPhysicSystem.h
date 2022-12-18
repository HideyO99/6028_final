#pragma once
#include <vector>
#include <map>
#include "cObject.h"
#include "cTriangle.h"
#include "../VAOManager/cModelDrawInfo.h"
#include "cBoundingBox.h"
#include "shape.h"

#define LO -1
#define HI  1
class cPhysicSystem
{
public:
	cPhysicSystem();
	~cPhysicSystem();

	//void createEnvironment(cModelDrawInfo drawInfo);
	void createObject(cMeshObj* meshObj, cModelDrawInfo* DrawInfo);
	void updateSystem(float dt);

	bool collisionCheck(cObject* pObjA, cObject* pObjB);

	bool gameUpdate();

	bool objPosUpdate();

	bool fire(glm::vec3 target);
	bool RayCastClosest(Ray ray, cObject** hitObject);
	bool RayCastFirstFound(Ray ray, cObject** hitObject);

	bool TestRaySphere(const Point& p, const Vector3& d, const Point& center, float radius);

	bool TestRayAABB(const Point& p, const Vector3& d, AABB aabb);

	bool TestSphereAABB(const Vector3& center, float radius, AABB b);

	float SqDistPointAABB(Vector3 p, AABB b);

	int TestSphereSphere(const Vector3& posA, float radiusA, const Vector3& posB, float radiusB);

	//cBoundingBox boundingBox;
	//cModelDrawInfo* pDrawInfo;
	//std::vector<cObject*> vec_Objects;
	//std::map<int, std::vector<cTriangle*>> mapEnvironmentAABBStructure;
	//std::map<int, std::vector<cTriangle*>> mapObjAABBStructure;
	std::map<std::string, cObject*> mapOBJ;
};

