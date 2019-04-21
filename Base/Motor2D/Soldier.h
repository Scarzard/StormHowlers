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

	void ChangeAnimation();

	void LoadAnimations(bool isPlayer1, string path);
	
};

#endif
