#pragma once
#include <string>

#define STARTINGID 100

struct sVector3
{
	sVector3() : x(0.f), y(0.f), z(0.f) {}
	float x, y, z;
};
class cGameObj
{
public:
	cGameObj();

	std::string name;
	float HP;
	sVector3 position;
	sVector3 velocity;

	unsigned int getID();

private:
	unsigned int ID;
	static unsigned int nextID;
};

