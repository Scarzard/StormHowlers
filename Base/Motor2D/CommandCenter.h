#ifndef __CmdCenter_H__
#define __CmdCenter_H__

#include "Building.h"

class CmdCenter : public Building
{
public:
	CmdCenter();
	CmdCenter(bool isPlayer1, pair<int, int> position);
	~CmdCenter();

	bool Update(float dt);
	void CleanUp();

	void LoadAnimations(bool isPlayer1, string path);

public:
	vector<uint> damage_cast2, damage_cast3;

};

#endif