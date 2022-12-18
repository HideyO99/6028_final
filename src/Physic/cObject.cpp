#include "cObject.h"

cObject::cObject()
{
	//position = &pMeshObj->position;
	isHover = false;
}

cObject::~cObject()
{
}

void cObject::setMass(float mass)
{
	invMass = 1.f / mass;
}

void cObject::integrate(float dT)
{
	acceleration = force * invMass;
	velocity = glm::vec3((acceleration.x * dT) + velocity.x, (acceleration.y * dT) + velocity.y, (acceleration.z * dT) + velocity.z);

	prevPosition = position;
	position = glm::vec3((velocity.x * dT) + position.x, (velocity.y * dT) + position.y, (velocity.z * dT) + position.z);
}

void cObject::applyForce(const glm::vec3& f_dir)
{
	force += f_dir;
}

void cObject::killAllForces()
{
	force = glm::vec3(0.f);
}

void cObject::update()
{
	//if (this->position.y < 0)
	//{
	//	this->position.y = 0;
	//}
	//if (this->position.y > 300)
	//{
	//	this->position.y = 300;
	//}
	if (this->position.x < -48)
	{
		this->position.x = -48;
	}
	if (this->position.x > 48)
	{
		this->position.x = 48;
	}

	if (this->position.z < -48)
	{
		this->position.z = -48;
	}
	if (this->position.z > 48)
	{
		this->position.z = 48;
	}
	//this->position.y = 30;
	pMeshObj->position = this->position;
	//this->pBBox->pMeshObj->position = this->position+ this->pBBox->centerPointOffset;
	this->pBBox->centerPoint = this->position + this->pBBox->centerPointOffset;
	this->pBBox->minPoint = this->position + this->pBBox->minPointOffset;
	this->pBBox->maxPoint = this->position + this->pBBox->maxPointOffset;
	this->pBBox->halfExtent = this->position + this->pBBox->halfExtentOffset;
}
