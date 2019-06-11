#ifndef __Group_H__
#define __Group_H__

#include <list>
#include "MovementManager.h"

class Troop;
class Entity;

class Group 
{
public:
	friend class MovementManager;

	Group();
	~Group();

	// --- Add/Remove ---
	void addUnit(Troop* unit_toadd);
	void removeUnit(Troop* unit_toremove);
	void AddTiletoOccupied(pair<int,int> to_add);

	// --- Clear Stuff ---
	void ClearGroup();
	void ClearOccupiedlist();

	// --- Getters ---
	int GetSize();

	// --- Check Movement Request ---
	bool CheckForMovementRequest(float dt, pair<int,int> destination);

	bool CheckForMovementIndividual(Troop * troop, float dt, pair<int, int> destination);

	// --- Enquiries ---
	bool IsGroupLead(Troop* entity);
	Troop* GetLead();
	void SetUnitGoalTile(Troop* entity);
	bool FindFreeAdjacents(pair<int,int> * base_tile);
	bool IsTileFree(pair<int,int>* adjacent);

	std::list <Troop*> Units;
private:
	pair<int,int> last_goal = { 0,0 };

	std::list <pair<int,int>*> Occupied_tiles;
};

struct Group_Unit
{
	bool IsSelected = false;
	bool everyone_arrived = false;
	Entity* closest = nullptr;
	//Entity* lead = nullptr;

	// --- Path to follow ---
	std::vector <pair<int,int>> Current_path;
	pair<int,int> next_tile = { 0,0 };
	pair<int,int> goal_tile = { 0,0 };

	// --- Group Movement stuff ---
	Group * current_group = nullptr;
	MovementState UnitMovementState = MovementState::MovementState_NoState;
};


#endif //__Group_H__
