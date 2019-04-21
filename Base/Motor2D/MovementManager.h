#ifndef __MovementManager_H__
#define __MovementManager_H__


#include "SDL\include\SDL.h"
#include "Module.h"
#include <list>

enum class MovementState
{
	MovementState_NoState,
	MovementState_Wait,
	MovementState_FollowPath,
	MovementState_NextStep,
	MovementState_DestinationReached
};

class Group;

class MovementManager : public Module
{
public:

	MovementManager();

	virtual ~MovementManager();

	// --- Called each loop iteration ---
	bool Update(float dt);

	// --- Called before quitting ---
	bool CleanUp();

	// --- Group Tools ---
	void SelectEntities_inRect(SDL_Rect SRect);
	void CreateGroup();

	// --- Move Unit ---
	void Move(Group* unit, float dt);

	int DistanceTo(pair<int, int> pos, pair<float, float> destination);

private:

	std::list <Group*>	Groups;
	bool stop_iteration = false;
};

#endif // __MovementManager_H__
