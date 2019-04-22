#include "Troop.h"
#include "Group.h"
#include "MovementManager.h"
#include "App.h"
#include "Input.h"
#include "Pathfinding.h"

Group::Group()
{
}

Group::~Group()
{
}

void Group::addUnit(Troop * unit_toadd)
{
	Units.push_back(unit_toadd);
}

void Group::removeUnit(Troop * unit_toremove)
{
	Units.remove(unit_toremove);
}

void Group::AddTiletoOccupied(pair<int,int> to_add)
{
	pair<int,int>* new_tile = new pair<int,int>;
	*new_tile = to_add;

	Occupied_tiles.push_back(new_tile);
}

void Group::ClearGroup()
{
	Units.clear();
}

void Group::ClearOccupiedlist()
{
	Occupied_tiles.clear();
}

int Group::GetSize()
{
	return Units.size();
}

void Group::CheckForMovementRequest(float dt)
{
 	//App->move_manager->Move(this,dt);
}

bool Group::IsGroupLead(Troop * entity)
{
	return (*Units.begin()) == entity;
}

void Group::SetUnitGoalTile(Troop* entity)
{
	std::list <Troop*>::const_iterator it = Units.begin();

	bool Goal_found = false;

	while (it != Units.end())
	{
		if (*it == entity)
		{
			it++;
			continue;
		}

		Goal_found = FindFreeAdjacents(&(*it)->info.goal_tile);

		if (Goal_found)
		{
			entity->info.goal_tile = last_goal;
			break;
		}

		it++;
	}

}

bool Group::FindFreeAdjacents(pair<int,int> * base_tile)
{
	bool ret = false;

	// --- Check if any adjacent tile to the base_tile is free ---

	pair<int,int> cell;

	// north
	cell = { base_tile->first, base_tile->second + 1 };
	if (App->pathfinding->IsWalkable(cell) && IsTileFree(&cell))
	{
		ret = true;
		last_goal = cell;
		return ret;
	}

	// north east
	cell = { base_tile->first + 1, base_tile->second + 1 };
	if (App->pathfinding->IsWalkable(cell) && IsTileFree(&cell))
	{
		ret = true;
		last_goal = cell;
		return ret;
	}

	// north west
	cell = { base_tile->first - 1, base_tile->second + 1 };
	if (App->pathfinding->IsWalkable(cell) && IsTileFree(&cell))
	{
		ret = true;
		last_goal = cell;
		return ret;
	}

	// south
	cell = { base_tile->first, base_tile->second - 1 };
	if (App->pathfinding->IsWalkable(cell) && IsTileFree(&cell))
	{
		ret = true;
		last_goal = cell;
		return ret;
	}

	// south east
	cell = { base_tile->first + 1, base_tile->second - 1 };
	if (App->pathfinding->IsWalkable(cell) && IsTileFree(&cell))
	{
		ret = true;
		last_goal = cell;
		return ret;
	}

	// south west
	cell = { base_tile->first - 1, base_tile->second - 1 };
	if (App->pathfinding->IsWalkable(cell) && IsTileFree(&cell))
	{
		ret = true;
		last_goal = cell;
		return ret;
	}

	// east
	cell = { base_tile->first + 1, base_tile->second };
	if (App->pathfinding->IsWalkable(cell) && IsTileFree(&cell))
	{
		ret = true;
		last_goal = cell;
		return ret;
	}

	// west
	cell = { base_tile->first - 1, base_tile->second };
	if (App->pathfinding->IsWalkable(cell) && IsTileFree(&cell))
	{
		ret = true;
		last_goal = cell;
		return ret;
	}


	return ret;
}

bool Group::IsTileFree(pair<int,int>* adjacent)
{
	std::list <pair<int,int>*>::const_iterator it = Occupied_tiles.begin();

	while (it != Occupied_tiles.end())
	{
		if (adjacent->first == (*it)->first && adjacent->second == (*it)->second)
		{
			return false;
		}

		it++;
	}

	AddTiletoOccupied(*adjacent);

	return true;
}

