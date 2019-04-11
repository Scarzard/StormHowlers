#ifndef __Soldier_H__
#define __Soldier_H__
#include "Entity.h"

class Soldier :	public Entity
{
public:
	Soldier();
	Soldier(bool isPlayer1, pair<int, int> pos);
	~Soldier();


	vector<uint> health_lv, upgrade_cost;
	vector<uint> damage_missiles, damage_cast2, damage_cast3;

	bool Update(float dt);

	void TakeDamage(int damage);


	SDL_Rect	collider;
	SDL_Texture* tex;

};

#endif
