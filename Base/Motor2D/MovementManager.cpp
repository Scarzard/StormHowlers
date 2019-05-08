#include "MovementManager.h"
#include "Entity.h"
#include "Group.h"
#include "App.h"
#include "EntityManager.h" 
#include "Log.h"
#include "Pathfinding.h"
#include "Scene.h"
#include "Map.h"
#include "Input.h"
#include "Brofiler\Brofiler.h"
#include "Render.h"
#include "Player.h"

MovementManager::MovementManager()
{
}

MovementManager::~MovementManager()
{
}

bool MovementManager::Update(float dt)
{

	return true;
}

bool MovementManager::CleanUp()
{
	std::list <Group*>::const_iterator group = Groups.begin();

	while (group != Groups.end())
	{
		(*group)->ClearGroup();
		delete *group;
		group++;
	}

	Groups.clear();

	return true;
}

void MovementManager::SelectEntities_inRect(SDL_Rect SRect)
{
	BROFILER_CATEGORY("GroupMovement::SelectEntities_inRect", Profiler::Color::DarkOliveGreen);

	std::list<Entity*>::iterator entity = App->entitymanager->entity_list.begin();
	SDL_Rect entityrect = { 0,0,0,0 };

	while (entity != App->entitymanager->entity_list.end())
	{
		entityrect = { (int)(*entity)->position.first, (int)(*entity)->position.second, (*entity)->size.first, (*entity)->size.second };

		// --- Check entity's rect against the given SRect, select it if overlap is positive ---
		if (SDL_HasIntersection(&entityrect, &SRect))
			(*entity)->isSelected = true;
		else
			(*entity)->isSelected = false;

		entity++;
	}

}

Group* MovementManager::CreateGroup(Player* player)
{
	BROFILER_CATEGORY("GroupMovement::CreateGroup", Profiler::Color::GhostWhite);

	bool Validgroup = false;

	Group* group = new Group;

	std::list<Troop*>::iterator entity = player->troops.begin();

	while (entity != player->troops.end())
	{

		// --- Check for entities that have been selected ---
		if ((*entity)->isSelected)
		{
			// --- If we find an entity then the group is valid and can be created ---
			Validgroup = true;

			// --- Remove the entity from a previous group if any is found ---
			if ((*entity)->info.current_group != nullptr)
			{
				(*entity)->info.current_group->removeUnit(*entity);

				if ((*entity)->info.current_group->GetSize() == 0)
					Groups.remove((*entity)->info.current_group);
			}

			// --- Add the entity to the new group, update its current group pointer ---
			group->addUnit(*entity);
			(*entity)->info.current_group = group;
		}
		(*entity)->isSelected = false;
		(*entity)->state = TROOP_IDLE;
		entity++;
	}

	// --- Finally, If the group is Valid add it to our Groups list, else delete it ---
	if (Validgroup) {
		Groups.push_back(group);
		return group;
	}
	else
		delete group;

}
/// Manages movement of groups
/** group to move, delta time, destination in world coordinates */
bool MovementManager::Move(Group * group, float dt, pair<int,int> destination)
{
	BROFILER_CATEGORY("GroupMovement::Move", Profiler::Color::Gold);

	std::list <Troop*>::const_iterator unit = group->Units.begin();

	//LOG("On Move Function");

	pair<int,int> Map_Entityposition;
	pair<float,float> distanceToNextTile;
	pair<int,int> next_tile_world;
	float DirectDistance;
	pair<float, float> to_fPoint;
	pair<int,int> goal_world;
	vector<pair<int, int>>::const_iterator it;

	// --- We get the map coords of the destinaiton ---
	/*pair<int,int> Map_mouseposition;
	App->input->GetMousePosition(Map_mouseposition.first, Map_mouseposition.second);
	Map_mouseposition = App->render->ScreenToWorld(Map_mouseposition.first, Map_mouseposition.second);*/

	destination = App->map->WorldToMap(destination.first, destination.second);

	/*if (unit == group->Units.begin()){
	bool everyone_arrived = true;
	while (unit != group->Units.end()) {
		if ((*unit)->info.UnitMovementState != MovementState::MovementState_NoState) {
			everyone_arrived = false;
			break;
		}
		unit++;
	}
	unit = group->Units.begin();
	}*/

	while (unit != group->Units.end())
	{
		if (unit._Ptr == nullptr)
		{
			unit++;
		}
		// --- We Get the map coords of the Entity ---
		Map_Entityposition.first = (*unit)->position.first;
		Map_Entityposition.second = (*unit)->position.second;
		Map_Entityposition = App->map->WorldToMap(Map_Entityposition.first, Map_Entityposition.second);

		//if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN && (*unit)->info.IsSelected)
	/*	if ((*unit)->info.UnitMovementState = MovementState::MovementState_NoState)
			(*unit)->info.UnitMovementState = MovementState::MovementState_NoState;
		*/
		switch ((*unit)->info.UnitMovementState)
		{

			case MovementState::MovementState_NoState:

				// --- On call to Move, Units will request a path to the destination ---

				if (true) {
				
				
					if (group->IsGroupLead((*unit)) == false)
					{
						// --- If any other unit of the group has the same goal, change the goal tile ---
						group->SetUnitGoalTile((*unit));
					}
					else
					{	
						// --- Clear previous path request occupied goal tiles ---
						group->ClearOccupiedlist();
						(*unit)->info.goal_tile = destination;
						group->Occupied_tiles.push_back(&(*unit)->info.goal_tile);
					}

					if (App->pathfinding->CreatePath(Map_Entityposition, (*unit)->info.goal_tile,true) > 0)
					{
						(*unit)->info.Current_path = *App->pathfinding->GetLastPath();
						(*unit)->info.Current_path.erase((*unit)->info.Current_path.begin());
						//(*unit)->info.Current_path.erase((*unit)->info.Current_path.begin());

						(*unit)->info.UnitMovementState = MovementState::MovementState_NextStep;
					}
					else {
						LOG("GroupMovement Error: Destination or Origin not Walkable");
						stop_iteration = true;
					}
				}

				break;

			case MovementState::MovementState_Wait:

				// --- Whenever the unit faces an obstacle of any type during a scheduled path, overcome it ---

				break;

			case MovementState::MovementState_FollowPath:

				// --- If a path is created, the unit will start following it ---
				next_tile_world = App->map->MapToWorld((*unit)->info.next_tile.first, (*unit)->info.next_tile.second);

				distanceToNextTile = { (float)next_tile_world.first - (*unit)->position.first,(float)next_tile_world.second - (*unit)->position.second };

				// --- We compute the module of our vector ---
				DirectDistance = sqrtf(pow(distanceToNextTile.first, 2.0f) + pow(distanceToNextTile.second, 2.0f));

				// --- We want a unitary vector to update the unit's direction/position ---
				if (DirectDistance > 0.0f)
				{
					distanceToNextTile.first /= DirectDistance;
					distanceToNextTile.second /= DirectDistance;
				}

				// --- Now we Apply the unit's Speed and the dt to the unitary vector  ---
				distanceToNextTile.first  *= (*unit)->speed*dt;
				distanceToNextTile.second *= (*unit)->speed*dt;

				// --- We convert an pair<int,int> to fPoint for comparing purposes ---
				to_fPoint.first = next_tile_world.first;
				to_fPoint.second = next_tile_world.second;

				if (DistanceTo((*unit)->position,to_fPoint) < 1)
				{
					(*unit)->position.first = next_tile_world.first;
					(*unit)->position.second = next_tile_world.second;

					(*unit)->info.UnitMovementState = MovementState::MovementState_NextStep;
				}

				else
				{
					(*unit)->position.first += distanceToNextTile.first;
					(*unit)->position.second += distanceToNextTile.second;

					(*unit)->Speed.first = distanceToNextTile.first;
					(*unit)->Speed.second = distanceToNextTile.second;
				}
			
				// --- Blit Unit's goal tile ---
				//goal_world = App->map->MapToWorld((*unit)->info.goal_tile.first, (*unit)->info.goal_tile.second);
				//App->render->Blit((*unit)->tex, goal_world.first, goal_world.second, &(*unit)->rect);

				// Show full path
				it = (*unit)->info.Current_path.begin();
				while (it != (*unit)->info.Current_path.end()) {
					goal_world = App->map->MapToWorld(it->first, it->second);
					App->render->Blit((*unit)->tex, goal_world.first, goal_world.second, &(*unit)->rect);
					it++;
				}


				it = (*unit)->info.Current_path.begin();
				while (it != (*unit)->info.Current_path.end()) {
					goal_world = App->map->MapToWorld(it->first, it->second);
					App->render->Blit((*unit)->tex, goal_world.first, goal_world.second, &(*unit)->rect);
					it++;
				}

				break;

			case MovementState::MovementState_NextStep:
				//(*unit)->state = MOVING;
				// --- If a path is being followed, the unit will get the next tile in the path ---

				if ((*unit)->info.Current_path.size() > 0)
				{
					(*unit)->info.next_tile = (*unit)->info.Current_path.front();
					(*unit)->info.Current_path.erase((*unit)->info.Current_path.begin());

					(*unit)->info.UnitMovementState = MovementState::MovementState_FollowPath;
				}
				else
				{
					(*unit)->info.UnitMovementState = MovementState::MovementState_DestinationReached;
				}

				break;

			case MovementState::MovementState_DestinationReached:

				// --- The unit reaches the end of the path, thus stopping and returning to NoState ---
				(*unit)->info.UnitMovementState = MovementState::MovementState_NoState;
				(*unit)->state = TROOP_IDLE;
				//(*unit)->isMoving = false;

				break;
		}


		if (stop_iteration)
		{
			stop_iteration = false;
			break;
		}

		unit++;

	}

	// Returns if everyone in group has arrived
	unit = group->Units.begin();
	while (unit != group->Units.end()) {
		if ((*unit)->info.UnitMovementState != MovementState::MovementState_NoState) {
			//(*unit)->info.everyone_arrived = false;
			return false;
		}
		unit++;
	}
	//(*unit)->info.everyone_arrived = true;
	return true;

}


int MovementManager::DistanceTo(pair<int,int> pos,pair<float, float> destination) {
	int fx = pos.first - destination.first;
	int fy = pos.second - destination.second;

	return int(sqrtf((fx*fx) + (fy*fy)));
}
