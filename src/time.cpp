#include "time.h"

cTime* cTime::instance;

void cTime::update()
{
	cTime::getInstance()->LastUpdate = cTime::getInstance()->CurrentUpdate;
	cTime::getInstance()->CurrentUpdate = std::chrono::steady_clock::now();
}

double cTime::getDeltaTime()
{
	std::chrono::duration<double> elapsed = cTime::getInstance()->CurrentUpdate - cTime::getInstance()->LastUpdate;
	return elapsed.count();
}

cTime* cTime::getInstance()
{
	if (instance == nullptr)
	{
		instance = new cTime();
	}
	return instance;
}
