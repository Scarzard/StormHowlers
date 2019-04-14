#ifndef __DefenseTarget_H__
#define __DefenseTarget_H__

#include "Building.h"
#include "Timer.h"

class DefenseTarget : public Building
{
public:
	DefenseTarget();
	DefenseTarget(bool isPlayer1, pair<int, int> pos);
	~DefenseTarget();

	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	void CleanUp();

	bool Is_inRange(pair<int, int> pos, int & distance);
	void LoadAnimations(bool isPlayer1, string path);
};

#endif