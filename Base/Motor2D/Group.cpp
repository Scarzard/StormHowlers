#include "Entity.h"
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

/** Removes unit from the group and sets a new lead

*/
void Group::removeUnit(Troop * unit_toremove)
{
	std::list <Troop*>::const_iterator unit = Units.begin();
	std::list <Troop*>::const_iterator unitEnd = Units.end();

	//Player* player = App->player1;
	//if (unit != unitEnd && !(*unit)->fromPlayer1) {
	//	player = App->player2;
	//}

	//std::list <Troop*>::const_iterator unitPlayer = player->troops.begin();
	//std::list <Troop*>::const_iterator unitPlayerEnd = player->troops.end();

	////DANGEROUS (?)
	//while (unitPlayer != unitPlayerEnd) {
	//	(*unitPlayer)->isSelected = false;
	//	unitPlayer++;
	//}

	while (unit != unitEnd) {
		//(*unit)->isSelected = true;
		if (*unit == unit_toremove) {
			Units.erase(unit);
			return;
		}
		unit++;
	}
	//unit_toremove->isSelected = false;

	//App->move_manager->CreateGroup(player);

	// TO DO : CHECK IF THE GROUP NEEDS TO BE DELETED
	//Units.remove(unit_toremove);
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

bool Group::CheckForMovementRequest(float dt,pair<int,int> destination)
{
 	return App->move_manager->Move(this,dt,destination);
}

/** The troop must be inside the group */
bool Group::CheckForMovementIndividual(Troop* troop, float dt, pair<int, int> destination)
{
	//Group* g = new Group();
	//g->Units.push_back(troop);

	removeUnit(troop);
	Group g;
	g.Units.push_back(troop);

	App->move_manager->Move(&g, dt, destination);
	
	g.ClearGroup();

	Units.push_back(troop);

	return true;
}
bool Group::IsGroupLead(Troop * entity)
{
	return (*Units.begin()) == entity;
}

Troop* Group::GetLead() {
	if (!Units.empty()) {
		return *Units.begin();
	}
	return nullptr;
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

