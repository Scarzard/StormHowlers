#include "Defs.h"
#include "Log.h"

#include "App.h"
#include "Player.h"
#include "Gui.h"
#include "Input.h"
#include "Window.h"
#include "EntityManager.h"
#include "Render.h"
#include "Pathfinding.h"

#include "Brofiler\Brofiler.h"


Player::Player() : Module()
{
}

Player::~Player()
{
}

bool Player::Start()
{
	gold = actual_capacity = 0;
	currentUI = CURR_MAIN;
	isBuilding = isDeploying = isCasting = false;
	currentTile = { 13,0 };

	return true;
}

bool Player::Update(float dt)
{
	BROFILER_CATEGORY("Player Update", Profiler::Color::Black);

	//--- Press X (Square)
	if (gamepad.Controller[BUTTON_X] == KEY_DOWN)
	{
		onUI = !onUI;
	}

	//--- Press B (Circle)
	if (gamepad.Controller[BUTTON_B] == KEY_DOWN || App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		if (currentUI == CURR_BUILD)
		{
			if (isBuilding == true)
			{
				isBuilding = false;
				App->map->debug = false;
			}
			else if (isBuilding == false)
			{
				currentUI = CURR_MAIN;
				UpdateVisibility();
			}
		}
	}

	//--- Building ---------------------
	if (isBuilding)
	{
		//--- Movement
		if (gamepad.Controller[JOY_UP] == KEY_REPEAT || gamepad.Controller[UP] == KEY_DOWN || 
			App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		{
			currentTile.second--;
		}
		else if (gamepad.Controller[JOY_DOWN] == KEY_REPEAT || gamepad.Controller[DOWN] == KEY_DOWN || 
			App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		{
			currentTile.second++;
		}

		if (gamepad.Controller[JOY_RIGHT] == KEY_REPEAT || gamepad.Controller[RIGHT] == KEY_DOWN ||
			App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			currentTile.first++;
		}
		else if (gamepad.Controller[JOY_LEFT] == KEY_REPEAT || gamepad.Controller[LEFT] == KEY_DOWN ||
			App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			currentTile.first--;
		}

		//--- Limits
		if (currentTile.first < x_limits.first) //left limit
		{
			currentTile.first = x_limits.first;
		}
		else if (currentTile.first > x_limits.second) //right limit
		{
			currentTile.first = x_limits.second;
		}
		
		if (currentTile.second < y_limits.first - collider.dimensions.first + collider.dimensions.second) //up limit
		{
			currentTile.second = y_limits.first - collider.dimensions.first + collider.dimensions.second;
		}
		else if (currentTile.second > y_limits.second - collider.dimensions.first + collider.dimensions.second) //down limit
		{
			currentTile.second = y_limits.second - collider.dimensions.first + collider.dimensions.second;
		}

		//--- Press A
		App->map->debug = true;
		if (CheckBuildingPos() == true) // Can build
		{
			if (gamepad.Controller[BUTTON_A] == KEY_DOWN || App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
			{
				//play fx (build);
				App->entitymanager->AddEntity(isPlayer1, type, { collider.tiles[0].first - offset.first, collider.tiles[0].second - offset.second });
				UpdateWalkabilityMap(false);
			}
		}
		else
		{
			if (gamepad.Controller[BUTTON_A] == KEY_DOWN || App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
			{
				//play fx (error);
			}
		}
	}
	return true;
}

bool Player::PostUpdate()
{
	BROFILER_CATEGORY("Player PostUpdate", Profiler::Color::Black);


	return true;
}

bool Player::CleanUp()
{
	LOG("---Player Deleted");

	//Clear buildings
	list<Entity*>::iterator item = buildings.begin();
	while (item != buildings.end())
	{
		(*item)->CleanUp();
		RELEASE(*item);
		item++;
	}
	buildings.clear();

	//Clear troops
	item = troops.begin();
	while (item != troops.end())
	{
		(*item)->CleanUp();
		RELEASE(*item);
		item++;
	}
	troops.clear();

	//Clear UI elements
	list<UI_Element*>::iterator item2 = UI_elements.begin();
	while (item2 != UI_elements.end())
	{
		(*item2)->children.clear();
		RELEASE(*item2);
		item2++;
	}
	UI_elements.clear();

	return true;
}

void Player::GetCurrentTile(int& x, int& y)
{
	x = currentTile.first;
	y = currentTile.second;
}

bool Player::CheckBuildingPos() // Check collider with walkability map
{
	bool ret = true;

	pair<int, int> pos, real_pos;
	pos = currentTile;

	// Check what tiles is the collider occupying
	int cont;
	collider.tiles.resize(0); //reset vector
	for (int i = 0; i < collider.dimensions.first; i++)
	{
		for (int j = 0; j < collider.dimensions.second; j++)
		{
			real_pos = App->map->MapToWorld(pos.first, pos.second);
			collider.tiles.push_back(real_pos); //add tile to collider.tiles
			pos.second++;
			cont = j;
		}
		pos.first++;
		pos.second -= cont + 1;
	}

	// compare tiles with walkability map
	for (int i = 0; i < collider.tiles.size(); i++)
	{
		pos = App->map->WorldToMap(collider.tiles[i].first, collider.tiles[i].second);
		if (App->pathfinding->IsWalkable(pos) == false)
		{
			ret = false;
			break;
		}
	}

	// Draw Collider
	SDL_Rect rect;
	for (int i = 0; i < collider.tiles.size(); i++)
	{
		if (ret == true)
			rect = { 0,0,60,29 }; //green
		else
			rect = { 60,0,60,29 }; //red

		App->render->Blit(App->map->debug_tex, collider.tiles[i].first, collider.tiles[i].second, &rect);
	}
	return ret;
}

void Player::UpdateWalkabilityMap(bool isWalkable) //update walkable tiles
{
	for (int i = 0; i < collider.tiles.size(); ++i)
	{
		pair <int,int> pos = App->map->WorldToMap(collider.tiles[i].first, collider.tiles[i].second);
		if (App->pathfinding->GetTileAt(pos) != isWalkable)
		{
			App->pathfinding->ChangeWalkability(pos, isWalkable);
		}
	}
}

void Player::UpdateVisibility() // Update GUI Visibility
{
	switch (currentUI)
	{
	case::Player::CURRENT_UI::CURR_MAIN:
		Main_UI->visible = true;
		Build_UI->visible = false;
		Deploy_UI->visible = false;
		Cast_UI->visible = false;
		//General_UI->visible = false;
		break;

	case::Player::CURRENT_UI::CURR_BUILD:
		Main_UI->visible = false;
		Build_UI->visible = true;
		Deploy_UI->visible = false;
		Cast_UI->visible = false;
		//General_UI->visible = false;
		break;

	case::Player::CURRENT_UI::CURR_DEPLOY:
		Main_UI->visible = false;
		Build_UI->visible = false;
		Deploy_UI->visible = true;
		Cast_UI->visible = false;
		//General_UI->visible = false;
		break;

	case::Player::CURRENT_UI::CURR_CAST:
		Main_UI->visible = false;
		Build_UI->visible = false;
		Deploy_UI->visible = false;
		Cast_UI->visible = true;
		//General_UI->visible = false;
		break;

	case::Player::CURRENT_UI::CURR_GENERAL:
		//Main_UI->visible = false;
		//Build_UI->visible = false;
		//Deploy_UI->visible = false;
		//Cast_UI->visible = false;
		//General_UI->visible = true;
		break;
	}
	App->gui->UpdateChildren();
}

void Player::DoLogic(UI_Element* data)
{
	switch (data->action)
	{
	case::UI_Element::Action::ACT_GOTO_MAIN:
		currentUI = CURR_MAIN;
		UpdateVisibility();
		break;

	case::UI_Element::Action::ACT_GOTO_BUILD:
		currentUI = CURR_BUILD;
		UpdateVisibility();
		break;

	case::UI_Element::Action::ACT_GOTO_DEPLOY:
		currentUI = CURR_DEPLOY;
		UpdateVisibility();
		break;

	case::UI_Element::Action::ACT_GOTO_CAST:
		currentUI = CURR_CAST;
		UpdateVisibility();
		break;

	case::UI_Element::Action::ACT_BUILD_AOE:
		isBuilding = true;
		type = Entity::entityType::DEFENSE_AOE;
		collider.dimensions = { 3,4 };
		//offset = { 60,30 };
		break;

	case::UI_Element::Action::ACT_BUILD_TARGET:
		isBuilding = true;
		type = Entity::entityType::DEFENSE_TARGET;
		//collider.dimensions = { 3,4 };
		//offset = { 60,30 };
		break;

	case::UI_Element::Action::ACT_BUILD_MINE:
		isBuilding = true;
		type = Entity::entityType::MINES;
		//collider.dimensions = { 3,4 };
		//offset = { 60,30 };
		break;

	case::UI_Element::Action::ACT_BUILD_BARRACKS:
		isBuilding = true;
		type = Entity::entityType::BARRACKS;
		//collider.dimensions = { 3,4 };
		//offset = { 60,30 };
		break;

	case::UI_Element::Action::ACT_DEPLOY_SOLDIER:
		//
		break;

	case::UI_Element::Action::ACT_DEPLOY_TANKMAN:
		//
		break;

	case::UI_Element::Action::ACT_DEPLOY_INFILTRATOR:
		//
		break;

	case::UI_Element::Action::ACT_DEPLOY_ENGINEER:
		//
		break;

	case::UI_Element::Action::ACT_DEPLOY_WARHOUND:
		//
		break;

	case::UI_Element::Action::ACT_CAST_MISSILES:
		//
		break;

	case::UI_Element::Action::ACT_CAST_2:
		//
		break;

	case::UI_Element::Action::ACT_CAST_3:
		//
		break;

	case::UI_Element::Action::ACT_UPGRADE:
		//
		break;

	case::UI_Element::Action::ACT_REPAIR:
		//
		break;
	}
}

bool Player::DeleteEntity(Entity* entity)
{
	entity->CleanUp();

	if (entity->type >= Entity::entityType::TOWNHALL && entity->type <= Entity::entityType::BARRACKS) //if entity = building
	{
		list<Entity*>::iterator item = buildings.begin();
		while (item != buildings.end())
		{
			if ((*item) == entity)
				buildings.erase(item);
		}
	}
	else if (type > Entity::entityType::BARRACKS) //if entity = troop
	{
		list<Entity*>::iterator item = troops.begin();
		while (item != troops.end())
		{
			if ((*item) == entity)
				troops.erase(item);
		}
	}
	return true;
}