#ifndef __DefenseTarget_H__
#define __DefenseTarget_H__
#include "Entity.h"
#include "Timer.h"

class DefenseTarget : public Entity
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


public:
	SDL_Rect	collider;
	SDL_Texture* tex;
};

#endif