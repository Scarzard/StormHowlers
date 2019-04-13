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
#include "MainMenu.h"

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


	// Button with focus changes state to HOVER 
	if (currentUI != CURRENT_UI::NONE && gamepad.Controller[BUTTON_A] != KEY_REPEAT && focus._Ptr != nullptr)
	{
		(*focus)->state = UI_Element::State::HOVER;
	}
	
	// Button A to clcik a button
	if (gamepad.Controller[BUTTON_A] == KEY_DOWN && currentUI != CURRENT_UI::NONE)
	{
		if(currentUI != CURRENT_UI::CURR_BUILD && currentUI != CURRENT_UI::CURR_DEPLOY && currentUI != CURRENT_UI::CURR_CAST)
			(*focus)->state = UI_Element::State::LOGIC;
	}
	
	if (gamepad.Controller[BUTTON_A] == KEY_UP && currentUI != CURRENT_UI::NONE)
	{
		if(!isBuilding)
			(*focus)->state = UI_Element::State::IDLE;
		if (App->scene->active)
			DoLogic((*focus));
		else
			App->main_menu->DoLogic((*focus));

		if((*focus)==Build_icon || (*focus) == Deploy_icon || (*focus) == Cast_icon)
			UpdateFocus(currentUI);
	}


	if (gamepad.Controller[BUTTON_B] == KEY_DOWN && currentUI != CURRENT_UI::NONE)
	{
		(*focus)->state = UI_Element::State::IDLE;


		if (currentUI == CURR_BUILD)
		{
			if (isBuilding == true)
			{
				isBuilding = false;
				App->map->debug = false;
			}
			else if (isBuilding == false)
			{

				GotoPrevWindows(currentUI);
				UpdateFocus(currentUI);
			}
		}
		else
		{
			GotoPrevWindows(currentUI);
			UpdateFocus(currentUI);
		}

		
	}

	if (gamepad.Controller[BUTTON_Y] == KEY_DOWN && currentUI == CURRENT_UI::NONE)
	{
		if(App->scene->active)
			currentUI = CURRENT_UI::CURR_MAIN;
		else if (App->main_menu->active)
			currentUI = CURRENT_UI::CURR_MAIN_MENU;

		UpdateFocus(currentUI);
	}

	if (gamepad.Controller[RB] == KEY_DOWN && currentUI != CURRENT_UI::NONE && gamepad.Controller[BUTTON_A] != KEY_REPEAT && isBuilding == false)
	{
		(*focus)->state = UI_Element::State::IDLE;

		if (focus == last_element)
		{
			focus = GetUI_Element(currentUI)->children.begin();
			
		}
		else
		{
			focus++;
		}
		
	}
	
	if (gamepad.Controller[LB] == KEY_DOWN && currentUI != CURRENT_UI::NONE && gamepad.Controller[BUTTON_A] != KEY_REPEAT && isBuilding == false)
	{
		(*focus)->state = UI_Element::State::IDLE;
		if (focus == GetUI_Element(currentUI)->children.begin())
		{
			focus = last_element;
		}
		else
		{
			focus--;
		}

	}


	


	// Just to test the LiveBar
	if (gamepad.Controller[UP] == KEY_DOWN)
	{
		
		live -= 100;
	}

	if (live < 0)
		live = 0;
	
	


	//--- Building ---------------------
	if (isBuilding)
	{
		//--- Movement
		if (gamepad.Controller[JOY_UP] == KEY_REPEAT || gamepad.Controller[UP] == KEY_DOWN || 
			App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_B) == KEY_REPEAT)
		{
			currentTile.second--;
		}
		else if (gamepad.Controller[JOY_DOWN] == KEY_REPEAT || gamepad.Controller[DOWN] == KEY_DOWN || 
		App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_G) == KEY_REPEAT)
		{
			currentTile.second++;
		}

		if (gamepad.Controller[JOY_RIGHT] == KEY_REPEAT || gamepad.Controller[RIGHT] == KEY_DOWN ||
			App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT)
		{
			currentTile.first++;
		}
		else if (gamepad.Controller[JOY_LEFT] == KEY_REPEAT || gamepad.Controller[LEFT] == KEY_DOWN ||
			App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_H) == KEY_REPEAT)
		{
			currentTile.first--;
		}

		//--- Limits
		//need to modify, when player1 is building we must chehk its width
		if (isPlayer1==true)
		{
			if (currentTile.first <= x_limits_player1.first) //left limit
			{
				currentTile.first = x_limits_player1.first;
			}
			else if (currentTile.first + collider.dimensions.first >= x_limits_player1.second) //right limit
			{
				currentTile.first = x_limits_player1.second- collider.dimensions.first;
			}

			if (currentTile.second < y_limits_player1.first ) //up limit
			{
				currentTile.second = y_limits_player1.first ;
			}
			else if (currentTile.second + collider.dimensions.second >= y_limits_player1.second) //down limit
			{
				currentTile.second = y_limits_player1.second - collider.dimensions.second;
			}
		}
		else if (isPlayer1 == false)
		{
			if (currentTile.first + collider.dimensions.first >= x_limits_player2.second) // right limit
			{
				currentTile.first = x_limits_player2.second-collider.dimensions.first;
			}
			else if (currentTile.first  <= x_limits_player2.first) //left limit
			{
				currentTile.first = x_limits_player2.first;
			}

			if (currentTile.second < y_limits_player2.first) //up limit
			{
				currentTile.second = y_limits_player2.first;
			}
			else if (currentTile.second + collider.dimensions.second >= y_limits_player2.second) //down limit
			{
				currentTile.second = y_limits_player2.second - collider.dimensions.second;
			}

		}
		
		// DEBUG PURPOSES DO NOT DELETE PLEASE
		if (App->input->GetKey(SDL_SCANCODE_B) == KEY_DOWN) {

			isBuilding = true;
			type = Entity::entityType::DEFENSE_TARGET;
			isPlayer1 = true;
			collider.dimensions = { 3,4 };
			//currentUI == CURRENT_UI::CURR_BUILD;
			//CheckBuildingPos();
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
	list<Building*>::iterator item = buildings.begin();
	while (item != buildings.end())
	{
		(*item)->CleanUp();
		RELEASE(*item);
		item++;
	}
	buildings.clear();

	//Clear troops
	list<Troop*>::iterator titem = troops.begin();
	while (titem != troops.end())
	{
		(*titem)->CleanUp();
		RELEASE(*titem);
		titem++;
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

	if (gamepad.Connected == true)
	{
		pos = currentTile;

	}
	else
	{
		App->input->GetMousePosition(pos.first, pos.second);
		pos = App -> render->ScreenToWorld(pos.first, pos.second);
		pos = App->map->WorldToMap(pos.first, pos.second);
		pos.first--;
	}
	
	
	


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
			App->map->walkability_layer->Set(pos.first, pos.second, 1);

		}
	}
}

void Player::UpdateFocus(uint data)
{
	switch (data)
	{
	case::Player::CURRENT_UI::CURR_MAIN_MENU:
		focus = App->main_menu->menu_background->children.begin();
		last_element = App->main_menu->menu_background->children.end();
		last_element--;
		break;
	case::Player::CURRENT_UI::CURR_MAIN:
		focus = Main_UI->children.begin();
		last_element = Main_UI->children.end();
		last_element--;
		break;

	case::Player::CURRENT_UI::CURR_BUILD:
		focus = Build_UI->children.begin();
		last_element = Build_UI->children.end();
		last_element--;
		break;

	case::Player::CURRENT_UI::CURR_CAST:
		focus = Cast_UI->children.begin();
		last_element = Cast_UI->children.end();
		last_element--;
		break;

	case::Player::CURRENT_UI::CURR_DEPLOY:
		focus = Deploy_UI->children.begin();
		last_element = Deploy_UI->children.end();
		last_element--;
		break;
	
	}
}


void Player::GotoPrevWindows(uint data)
{
	switch (data)
	{
	case Player::CURRENT_UI::CURR_MAIN :
		currentUI = CURRENT_UI::NONE;
		UpdateVisibility();
		break;

	case Player::CURRENT_UI::CURR_GENERAL:
		currentUI = CURRENT_UI::NONE;
		UpdateVisibility();
		break;

	case Player::CURRENT_UI::CURR_BUILD :
		currentUI = CURRENT_UI::CURR_MAIN;
		UpdateVisibility();
		break;

	case Player::CURRENT_UI::CURR_DEPLOY:
		currentUI = CURRENT_UI::CURR_MAIN;
		UpdateVisibility();
		break;

	case Player::CURRENT_UI::CURR_CAST:
		currentUI = CURRENT_UI::CURR_MAIN;
		UpdateVisibility();
		break;

	}
}

UI_Element* Player::GetUI_Element(uint data)
{
	switch (data)
	{
	case::Player::CURRENT_UI::CURR_MAIN_MENU:
		return App->main_menu->menu_background;
	case::Player::CURRENT_UI::CURR_MAIN:
		return Main_UI;

	case::Player::CURRENT_UI::CURR_BUILD:
		return Build_UI;

	case::Player::CURRENT_UI::CURR_CAST:
		return Cast_UI;

	case::Player::CURRENT_UI::CURR_DEPLOY:
		return Deploy_UI;

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
		Deploy_UI->visible = true;
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

		type = Entity::entityType::DEFENSE_AOE;
		collider.dimensions = { 3,4 };
		break;

	case::UI_Element::Action::ACT_BUILD_MINE:
		isBuilding = true;

		type = Entity::entityType::DEFENSE_AOE;
		collider.dimensions = { 3,4 };

		break;

	case::UI_Element::Action::ACT_BUILD_BARRACKS:
		isBuilding = true;

		type = Entity::entityType::DEFENSE_AOE;
		collider.dimensions = { 3,4 };

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
		list<Building*>::iterator item = buildings.begin();
		while (item != buildings.end())
		{
			if ((*item) == entity)
				buildings.erase(item);
		}
	}
	else if (type > Entity::entityType::BARRACKS) //if entity = troop
	{
		list<Troop*>::iterator item = troops.begin();
		while (item != troops.end())
		{
			if ((*item) == entity)
				troops.erase(item);
		}
	}
	return true;
}