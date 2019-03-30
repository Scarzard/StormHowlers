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
	gold = 0;
	currentUI = CURR_MAIN;
	isBuilding = false;
	col = { 0,0,0,0 };
	quadSize = { 0,0 };
	tex = App->tex->Load("maps/pathfinding.png");

	return true;
}

bool Player::Update(float dt)
{
	BROFILER_CATEGORY("Player Update", Profiler::Color::Black);

	//--- Cursor Movement
	if (gamepad.Controller[JOY_UP] == KEY_REPEAT || gamepad.Controller[UP] == KEY_REPEAT)
		cursor.position.second -= 500 * dt;

	if (gamepad.Controller[JOY_DOWN] == KEY_REPEAT || gamepad.Controller[DOWN] == KEY_REPEAT)
		cursor.position.second += 500 * dt;

	if (gamepad.Controller[JOY_RIGHT] == KEY_REPEAT || gamepad.Controller[RIGHT] == KEY_REPEAT)
		cursor.position.first += 500 * dt;

	if (gamepad.Controller[JOY_LEFT] == KEY_REPEAT || gamepad.Controller[LEFT] == KEY_REPEAT)
		cursor.position.first -= 500 * dt;

	//--- Press A
	if (gamepad.Controller[BUTTON_A] == KEY_DOWN)
	{
		if (isBuilding) //Building
		{
			col = { cursor.position.first, cursor.position.second, quadSize.first, quadSize.second };
			if (CheckBuildingPos(col) == true) // Can build
			{
				//play fx (build)
				App->entitymanager->AddEntity(isPlayer1, type, { col.x, col.y });
			}
			else
			{
				//play fx (error)
			}
		}
	}

	//--- Press B
	if (gamepad.Controller[BUTTON_B] == KEY_DOWN)
	{
		if (currentUI == CURR_BUILD)
		{
			if (isBuilding == true)
			{
				isBuilding = false;
			}
			else if (isBuilding == false)
			{
				currentUI = CURR_MAIN;
				UpdateVisibility();
			}
		}
	}

	return true;
}

bool Player::PostUpdate()
{
	BROFILER_CATEGORY("Player PostUpdate", Profiler::Color::Black);

	if (isBuilding)
	{
		if (gamepad.Connected == false)
		{
			int x, y;
			App->input->GetMousePosition(x, y);
			col = { x, y, quadSize.first, quadSize.second };
		}
		else
		{
			col = { cursor.position.first, cursor.position.second, quadSize.first, quadSize.second };
		}

		if (CheckBuildingPos(col) == true)
			App->render->DrawQuad(col, 0, 255, 0, 50); //green	
		else
			App->render->DrawQuad(col, 255, 0, 0, 50); //red	
	}

	return true;
}

bool Player::CleanUp()
{
	LOG("---Player Deleted");

	//Clear UI elements
	list<UI_Element*>::iterator item = UI_elements.begin();
	while (item != UI_elements.end())
	{
		(*item)->children.clear();
		RELEASE(*item);
		item++;
	}
	UI_elements.clear();

	return true;
}

void Player::GetCursorPos(int& x, int& y)
{
	x = cursor.position.first;
	y = cursor.position.second;
}

bool Player::CheckCursorPos(UI_Element* data)
{
	bool ret = false;

	int x, y;
	GetCursorPos(x, y);
	SDL_Rect CursorCollider = { x,y,1,1 };

	if (SDL_HasIntersection(&CursorCollider, &data->collider))
		ret = true;

	return ret;
}

bool Player::CheckCursorClick(UI_Element* data)
{
	bool ret = false;

	if (gamepad.Controller[BUTTON_A] == KEY_DOWN)
	{
		GetCursorPos(data->click_pos.first, data->click_pos.second);
		data->start_drag_pos = data->globalpos;
		ret = true;
	}
	if (gamepad.Controller[BUTTON_A] == KEY_REPEAT)
	{
		ret = true;
	}
	if (gamepad.Controller[BUTTON_A] == KEY_UP)
	{
		data->dragging = false;
		return false;
	}

	return ret;
}

bool Player::CheckBuildingPos(SDL_Rect collider) //check collider with walkability map
{
	//check with preset objects in map
	SDL_Rect collisions;
	for (list<ObjectsGroup*>::iterator object = App->map->data.objLayers.begin(); object != App->map->data.objLayers.end(); object++)
	{
		if ((*object)->name == ("Collision"))
		{
			for (list<ObjectsData*>::iterator objectdata = (*object)->objects.begin(); objectdata != (*object)->objects.end(); objectdata++)
			{
				collisions.x = (*objectdata)->x;
				collisions.y = (*objectdata)->y;
				collisions.w = (*objectdata)->width;
				collisions.h = (*objectdata)->height;

				if (SDL_HasIntersection(&collisions, &collider) == true)
					return false;
			}
		}
	}
	//check with list of entities
	for (list<Entity*>::iterator item = App->entitymanager->Entities.begin(); item != App->entitymanager->Entities.end(); item++)
	{
		if (SDL_HasIntersection(&(*item)->Collider, &collider) == true)
			return false;
	}

	return true;
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
		quadSize = { 50, 50 };
		break;

	case::UI_Element::Action::ACT_BUILD_TARGET:
		//
		break;

	case::UI_Element::Action::ACT_BUILD_MINE:
		//
		break;

	case::UI_Element::Action::ACT_BUILD_BARRACKS:
		//
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