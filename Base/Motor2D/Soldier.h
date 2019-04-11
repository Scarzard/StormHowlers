#ifndef __Soldier_H__
#define __Soldier_H__
#include "Entity.h"

class Soldier :	public Entity
{
public:
	Soldier();
	Soldier(bool isPlayer1, pair<int, int> pos);
	~Soldier();

	bool Update(float dt);

	SDL_Rect	collider;
	SDL_Texture* tex;

};

#endif
