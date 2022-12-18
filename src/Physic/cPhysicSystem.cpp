#include "cPhysicSystem.h"
#include "math.h"

cPhysicSystem::cPhysicSystem()
{
}

cPhysicSystem::~cPhysicSystem()
{
}

int CalculateHashValue(const glm::vec3& vec)
{
    int hashValue = 0;

    hashValue += ((int)(floor(vec.x + 250) / 10)) * 100000;
    hashValue += ((int)(floor(vec.y) / 10)) *1000;
    hashValue += ((int)(floor(vec.z + 250) / 10)) ;
    return hashValue;
}

// Test if AABB b intersects plane P
int TestAABBPlane(cBoundingBox b, sPlane p)
{
    // These two lines not necessary with a (Center, extents) AABB representation
    glm::vec3 c = (b.maxPoint + b.minPoint) * 0.5f; // Compute AABB center
    glm::vec3 e = b.maxPoint - c; // Compute positive extents
    // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
    float r = e.x * std::abs(p.n.x) + e.y * std::abs(p.n.y) + e.z*(std::abs(p.n.z));
    // Compute distance of box center from plane
    float s = Dot(p.n, c) - p.d;
    // Intersection occurs when distance s falls within [-r, +r] interval
    return std::abs(s) <= r;
}

int TestTriangleAABB(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, cBoundingBox b)
{
    float p0, p1, p2, r;
    // compute box center and extents (if not already given in that format)
    glm::vec3 c = (b.minPoint + b.maxPoint) * 0.5f;
    float e0 = (b.maxPoint.x - b.minPoint.x) * 0.5f;
    float e1 = (b.maxPoint.y - b.minPoint.y) * 0.5f;
    float e2 = (b.maxPoint.z - b.minPoint.z) * 0.5f;
    // Translate triangle as conceptually moving AABB to origin
    v0 = v0 - c;
    v1 = v1 - c;
    v2 = v2 - c;
    // Compute edge vectors for triangle
    glm::vec3 f0 = v1 - v0, f1 = v2 - v1, f2 = v0 - v2;
    // Test axes a00..a22 (category 3)
    glm::vec3 a[3][3];

    a[0][0] = glm::vec3(0.f, -f0.z, f0.y);
    a[0][1] = glm::vec3(0.f, -f1.z, f1.y);
    a[0][2] = glm::vec3(0.f, -f2.z, f2.y);
     
    a[1][0] = glm::vec3(f0.z, 0.f, -f0.x);
    a[1][1] = glm::vec3(f1.z, 0.f, -f1.x);
    a[1][2] = glm::vec3(f2.z, 0.f, -f2.x);
     
    a[2][0] = glm::vec3(-f0.y, f0.x, 0.f);
    a[2][0] = glm::vec3(-f1.y, f1.x, 0.f);
    a[2][0] = glm::vec3(-f2.y, f2.x, 0.f);


    r = e1 * std::abs(f0.z) + e2 * std::abs(f0.y);
    for (size_t i = 0; i < 3; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            p0 = Dot(v0, a[i][j]);
            p1 = Dot(v1, a[i][j]);
            p2 = Dot(v2, a[i][j]);
            
            if (std::fmaxf(-(std::fmaxf(p0, p2)), std::fminf(p0, p2)) > r)
            return 0; // axis is a separating axis

        }
    }

        // Test the three axes corresponding to the face normals of AABB b (category 1).
        // Exit if ...
        // ... [-e0, e0] and [min(v0.x, v1.x, v2.x), max(v0.x, v1.x, v2.x)] do not overlap
    if (std::fmaxf(v0.x, std::fmaxf(v1.x, v2.x)) < -e0 || std::fminf(v0.x, std::fminf(v1.x, v2.x)) > e0) return 0;
    // ... [-e1, e1] and [min(v0.y, v1.y, v2.y), max(v0.y, v1.y, v2.y)] do not overlap
    if (std::fmaxf(v0.y, std::fmaxf(v1.y, v2.y)) < -e1 || std::fminf(v0.y, std::fminf(v1.y, v2.y)) > e1) return 0;
    // ... [-e2, e2] and [min(v0.z, v1.z, v2.z), max(v0.z, v1.z, v2.z)] do not overlap
    if (std::fmaxf(v0.z, std::fmaxf(v1.z, v2.z)) < -e2 || std::fminf(v0.z, std::fminf(v1.z, v2.z)) > e2) return 0;
    // Test separating axis corresponding to triangle face normal (category 2)
    sPlane p;
    p.n = Cross(f0, f1);
    p.d = Dot(p.n, v0);
    return TestAABBPlane(b, p);
}

void cPhysicSystem::createObject(cMeshObj* meshObj,cModelDrawInfo* DrawInfo)
{
    cObject* obj = new cObject();
    obj->pMeshObj = meshObj;
    //pDrawInfo = DrawInfo;
    cBoundingBox* pBBox = new cBoundingBox();
    obj->pBBox = pBBox;
    //plane->setMass(1.f);
    //plane->pBBox = &boundingBox;
    //plane->position = glm::vec3(0, 100,0);
    obj->position = meshObj->position;
    //pBBox->halfExtentOffset = glm::vec3((DrawInfo->extentX / 2.f) * meshObj->scale.x, (DrawInfo->extentY / 2.f) * meshObj->scale.y, (DrawInfo->extentZ / 2.f) * meshObj->scale.z);
    ////float x = pBBox->halfExtentOffset.x + (DrawInfo->minX * meshObj->scale.x);
    ////float y = pBBox->halfExtentOffset.y + (DrawInfo->minY * meshObj->scale.y);
    ////float z = pBBox->halfExtentOffset.z + (DrawInfo->minZ * meshObj->scale.z);
    //pBBox->centerPointOffset = glm::vec3(pBBox->halfExtentOffset.x + (DrawInfo->minX * meshObj->scale.x), pBBox->halfExtentOffset.y + (DrawInfo->minY * meshObj->scale.y), pBBox->halfExtentOffset.z + (DrawInfo->minZ * meshObj->scale.z));
    //pBBox->maxPointOffset = glm::vec3(DrawInfo->maxX * meshObj->scale.x, DrawInfo->maxY * meshObj->scale.y, DrawInfo->maxZ * meshObj->scale.z);
    //pBBox->minPointOffset = glm::vec3(DrawInfo->minX * meshObj->scale.x, DrawInfo->minY * meshObj->scale.y, DrawInfo->minZ * meshObj->scale.z);

    //if (meshObj->meshName == "enemy")
    //{
    //    Sphere* enemy = new Sphere(Point(0), DrawInfo->extentY/2);
    //    obj->pShape = enemy;
    //}
    //if (meshObj->meshName == "player")
    //{
    //    float min[3] = { DrawInfo->minX,DrawInfo->minY,DrawInfo->minZ };
    //    float max[3] = { DrawInfo->maxX,DrawInfo->maxY,DrawInfo->maxZ };
    //    AABB* player = new AABB( min,max );

    //    obj->pShape = player;
    //}
    //if (meshObj->meshName == "obstacle")
    //{
    //    float min[3] = { pBBox->minPointOffset.x + meshObj->position.x,pBBox->minPointOffset.y + meshObj->position.y,pBBox->minPointOffset.z + meshObj->position.z };
    //    float max[3] = { pBBox->maxPointOffset.x + meshObj->position.x,pBBox->maxPointOffset.y + meshObj->position.y,pBBox->maxPointOffset.z + meshObj->position.z };
    //    AABB* obstacle = new AABB(min, max);

    //    obj->pShape = obstacle;
    //}

    mapOBJ.emplace(obj->pMeshObj->instanceName, obj);

}

void cPhysicSystem::updateSystem(float dt)
{
    objPosUpdate();

    std::map<std::string, cObject*>::iterator playerObj = mapOBJ.find("Player");

    for (std::map<std::string, cObject*>::iterator obj_it = mapOBJ.begin(); obj_it != mapOBJ.end(); obj_it++)
    {
        if (obj_it->second->pMeshObj->meshName == "enemy")
        {
            bool tmp =
                collisionCheck(playerObj->second, obj_it->second);
            if (tmp)
            {
                obj_it->second->isHover = true;
            }
            else
            {
                obj_it->second->isHover = false;
            }
        }
    }

}

bool cPhysicSystem::collisionCheck(cObject* pObjA, cObject* pObjB)
{
    bool isCollision = false;

    Sphere* pEnemy = dynamic_cast<Sphere*>(pObjB->pShape);
    
    Vector3 posB = pEnemy->Center + Vector3(pObjB->position.x, pObjB->position.y, pObjB->position.z);
    Vector3 posA = Vector3(pObjA->position.x, pObjA->position.y, pObjA->position.z);
    isCollision = TestSphereSphere(posB, pEnemy->Radius, posA,1); //hack assume player is sphere

    return isCollision;
}

bool cPhysicSystem::gameUpdate()
{

    for (std::map<std::string, cObject*>::iterator obj_it = mapOBJ.begin(); obj_it != mapOBJ.end(); obj_it++)
    {
        if (obj_it->second->pMeshObj->meshName == "enemy")
        {
            if (!obj_it->second->pMeshObj->isVisible)
            {
                //respawn
                obj_it->second->position = glm::vec3(0.f,1.f,0.f);
                obj_it->second->pMeshObj->isVisible = true;
            }
            if (obj_it->second->isHover)
            {
                obj_it->second->position = glm::vec3(-10000.f, -100000.f, -10000.f);
                obj_it->second->pMeshObj->isVisible = false;
                obj_it->second->isHover = false;
            }
            int dirX = (rand() % 100) - 50;
            int dirZ = (rand() % 100) - 50;
            obj_it->second->direction += glm::normalize(glm::vec3(dirX, 0, dirZ));
        }
    }
    return true;
}

bool cPhysicSystem::objPosUpdate()
{
    float step = 0.1f;
    for (std::map<std::string, cObject*>::iterator obj_it = mapOBJ.begin(); obj_it != mapOBJ.end(); obj_it++)
    {
        if (obj_it->second->pMeshObj->meshName == "enemy")
        {
            obj_it->second->position += obj_it->second->direction * step;
            obj_it->second->update();

        }
        if (obj_it->second->pMeshObj->meshName == "bullet")
        {
            obj_it->second->position += obj_it->second->direction * 10.f;
            obj_it->second->update();
        }
    }

    return true;
}

bool cPhysicSystem::fire(glm::vec3 target)
{
    //float step = 0.1f;
    std::map<std::string, cObject*>::iterator obj_it = mapOBJ.find("bullet");
    

    obj_it->second->direction = target;
    cObject* hitObj;
    Ray ray(obj_it->second->position, obj_it->second->direction);
    if (RayCastClosest(ray, &hitObj))
    {
        hitObj->position = glm::vec3(-10000.f, -100000.f, -10000.f);
        hitObj->pMeshObj->isVisible = false;
    }
    
    return true;
}

bool cPhysicSystem::RayCastClosest(Ray ray, cObject** hitObject)
{
    cObject* closestObj = nullptr;
    float closestDistance = FLT_MAX;

    for (std::map<std::string, cObject*>::iterator obj_it = mapOBJ.begin(); obj_it != mapOBJ.end(); obj_it++)
    {
        if ((obj_it->second->pMeshObj->meshName != "bullet") && (obj_it->second->pMeshObj->meshName != "player"))
        {
            float distance = Vector3::Distance(ray.origin, obj_it->second->position);
            if (obj_it->second->pShape->getShapeType() == TYPE_AABB)
            {
                AABB* pAABB = dynamic_cast<AABB*>(obj_it->second->pShape);
                if (TestRayAABB(ray.origin, ray.direction, *pAABB))
                {
                    //float distance = Vector3::Distance(ray.origin, obj_it->second->position);
                    if (distance < closestDistance)
                    {
                        closestObj = nullptr;
                        closestDistance = distance;
                    }
                }
            }
            if (obj_it->second->pShape->getShapeType() == TYPE_SPHERE)
            {
                Sphere* pSphere = dynamic_cast<Sphere*>(obj_it->second->pShape);
                if (TestRaySphere(ray.origin, ray.direction, pSphere->Center + obj_it->second->position, pSphere->Radius))
                {
                    if (distance < closestDistance)
                    {
                        closestObj = obj_it->second;
                        closestDistance = distance;
                    }
                }
            }
        }
    }

    *hitObject = closestObj;

    return closestObj!=nullptr;
}

bool cPhysicSystem::RayCastFirstFound(Ray ray, cObject** hitObject)
{
    for (std::map<std::string, cObject*>::iterator obj_it = mapOBJ.begin(); obj_it != mapOBJ.end(); obj_it++)
    {
        if (obj_it->second->pShape->getShapeType() == TYPE_SPHERE)
        {
            Sphere* pSphere = dynamic_cast<Sphere*>(obj_it->second->pShape);
            if (TestRaySphere(ray.origin, ray.direction, pSphere->Center + obj_it->second->position, pSphere->Radius))
            {
                *hitObject = obj_it->second;
                return true;
            }
        }
    }

    return false;
}

bool cPhysicSystem::TestRaySphere(const Point& p, const Vector3& d, const Point& center, float radius)
{
    Point diff = p - center;
    glm::vec3 m = glm::vec3(diff.x,diff.y,diff.z);
    float c = Dot(m, m) - radius * radius;
    

    // intersection
    if (c <= 0.0f) return true;
    float b = Dot(m, glm::vec3(d.x,d.y,d.z));

    // ray origin outside sphere and ray pointing away from sphere
    if (b > 0.0f) return false;

    float disc = b * b - c;

    //  ray missing sphere
    if (disc < 0.0f) return false;

    // Now ray must hit sphere
    return true;
}
bool cPhysicSystem::TestRayAABB(const Point& p, const Vector3& d, AABB aabb)
{
    Vector3 dirfrac;
    
    dirfrac.x = 1.0f / d.x;
    dirfrac.y = 1.0f / d.y;
    dirfrac.z = 1.0f / d.z;
    // lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
    // r.org is origin of ray
    float t1 = (aabb.Min[0] - p.x) * dirfrac.x;
    float t2 = (aabb.Max[0] - p.x) * dirfrac.x;
    float t3 = (aabb.Min[1] - p.y) * dirfrac.y;
    float t4 = (aabb.Max[1] - p.y) * dirfrac.y;
    float t5 = (aabb.Min[2] - p.z) * dirfrac.z;
    float t6 = (aabb.Max[2] - p.z) * dirfrac.z;

    float tmin = fmax(fmax(fmin(t1, t2), fmin(t3, t4)), fmin(t5, t6));
    float tmax = fmin(fmin(fmax(t1, t2), fmax(t3, t4)), fmax(t5, t6));

    // if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
    if (tmax < 0)
    {
        //t = tmax;
        return false;
    }

    // if tmin > tmax, ray doesn't intersect AABB
    if (tmin > tmax)
    {
       // t = tmax;
        return false;
    }

    //t = tmin;
    return true;
}
bool cPhysicSystem::TestSphereAABB(const Vector3& center, float radius, AABB b)
{
    float sqDist = SqDistPointAABB(center, b);
    return sqDist <= radius * radius;
}

float cPhysicSystem::SqDistPointAABB(Vector3 p, AABB b)
{
    float sqDist = 0.0f;

    float v;
    v = p.x;
    if (v < b.Min[0]) sqDist += (b.Min[0] - v) * (b.Min[0] - v);
    if (v > b.Max[0]) sqDist += (v - b.Max[0]) * (v - b.Max[0]);

    v = p.y;
    if (v < b.Min[1]) sqDist += (b.Min[1] - v) * (b.Min[1] - v);
    if (v > b.Max[1]) sqDist += (v - b.Max[1]) * (v - b.Max[1]);

    v = p.z;
    if (v < b.Min[2]) sqDist += (b.Min[2] - v) * (b.Min[2] - v);
    if (v > b.Max[2]) sqDist += (v - b.Max[2]) * (v - b.Max[2]);

    return sqDist;
}

int cPhysicSystem::TestSphereSphere(const Vector3& posA, float radiusA, const Vector3& posB, float radiusB)
{
    // Calculate squared distance between centers
    Vector3 d = posA - posB;
    glm::vec3 tmp = glm::vec3(d.x, d.y, d.z);
    float dist2 = Dot(tmp, tmp);

    // Spheres intersect if squared distance is less
    // than squared sum of radii
    float radiusSum = radiusA + radiusB;
    return dist2 <= radiusSum * radiusSum;
}
