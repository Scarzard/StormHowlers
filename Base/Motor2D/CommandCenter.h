#ifndef __CmdCenter_H__
#define __CmdCenter_H__
#include "Entity.h"

class CmdCenter : public Entity
{
public:
	CmdCenter();
	CmdCenter(bool isPlayer1, pair<int, int> position);
	~CmdCenter();

	bool Update(float dt);
	void CleanUp();


public:
	vector<uint> damage_cast2, damage_cast3;

};

#endif