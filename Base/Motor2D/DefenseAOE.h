#ifndef __DefenseAOE_H__
#define __DefenseAOE_H__

#include "Building.h"
#include "Timer.h"

struct Closest
{
	bool operator()(pair<Troop*,int> const & troop1, pair<Troop*, int> const & troop2) //true if distance is smaller
	{
		if (troop1.second <= troop2.second)
			return true;
		else
			return false;
	}
};

class DefenseAoe : public Building
{
public:
	DefenseAoe();
	DefenseAoe(bool isPlayer1, pair<int, int> pos, Collider collider);
	~DefenseAoe();

	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	void CleanUp();

	bool Is_inRange(pair<int, int> pos, int & distance);
	void LoadAnimations(bool isPlayer1, string path);

public:
	Collider	colider;
	SDL_Texture* tex = nullptr;

private:
	bool first_shot;
};

#endif