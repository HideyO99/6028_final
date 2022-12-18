#include "cGameObj.h"

cGameObj::cGameObj()
{
	this->ID = cGameObj::nextID;
	cGameObj::nextID++;
	this->HP = 100.f;
	this->pMeshObj = nullptr;
	cmdIndex = 0;
}

unsigned int cGameObj::nextID = STARTINGID;


unsigned int cGameObj::getID()
{
	return this->ID;
}
