#pragma once
#include <chrono>

class cTime
{
public:
	static void update();
	static double getDeltaTime();

	static cTime* instance;
	static cTime* getInstance();

	std::chrono::steady_clock::time_point LastUpdate;
	std::chrono::steady_clock::time_point CurrentUpdate;
};

