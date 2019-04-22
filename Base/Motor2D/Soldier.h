#ifndef __Soldier_H__
#define __Soldier_H__
#include "Troop.h"
#include "EntityManager.h"

class Soldier :	public Troop
{
public:
	Soldier();
	Soldier(bool isPlayer1, pair<int, int> pos, Collider collider);
	~Soldier();

	bool Update(float dt);
	void CleanUp();

	void ChangeAnimation(pair<int, int> Speed, Entity * closest);

	void ChangeAnimation();

	void LoadAnimations(bool isPlayer1, string path);

	bool Is_inRange(pair<int, int> pos, int & distance);
	
};

#endif
