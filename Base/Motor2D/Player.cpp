#include "Defs.h"
#include "Log.h"

#include "App.h"
#include "Player.h"
#include "Gui.h"
#include "Input.h"
#include "Window.h"

#include "SDL/include/SDL.h"
#include "Brofiler\Brofiler.h"


Player::Player() : Module()
{
}

Player::~Player()
{
}

bool Player::Awake(pugi::xml_node & config)
{
	LOG("Loading Player");
	bool ret = true;

	// Load Main Buildings starting position
	config = config.child("player");

	return ret;
}

bool Player::Start()
{
	// Create Main Buildings

	return true;
}

bool Player::Update(float dt)
{
	BROFILER_CATEGORY("Player Update", Profiler::Color::Black);

	//--- Update Cursor
	if (gamepad.Controller[JOY_UP] == KEY_REPEAT || gamepad.Controller[UP] == KEY_REPEAT)
		cursor.position.second -= 500 * dt;

	if (gamepad.Controller[JOY_DOWN] == KEY_REPEAT || gamepad.Controller[DOWN] == KEY_REPEAT)
		cursor.position.second += 500 * dt;

	if (gamepad.Controller[JOY_RIGHT] == KEY_REPEAT || gamepad.Controller[RIGHT] == KEY_REPEAT)
		cursor.position.first += 500 * dt;

	if (gamepad.Controller[JOY_LEFT] == KEY_REPEAT || gamepad.Controller[LEFT] == KEY_REPEAT)
		cursor.position.first -= 500 * dt;

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
		//
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