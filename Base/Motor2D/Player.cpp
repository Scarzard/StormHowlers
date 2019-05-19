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
#include "Fonts.h"
#include "Audio.h"
#include "Scene.h"
#include "Transitions.h"


#include "Brofiler\Brofiler.h"


Player::Player(bool isplayer) : Module()
{
	name = "player";
	isPlayer1 = isplayer;
}

Player::Player() : Module()
{
	name = "player";
}

Player::~Player()
{
}

bool Player::Awake(pugi::xml_node& config) {
	preview_rects = new vector<SDL_Rect>(Entity::entityType::WAR_HOUND, SDL_Rect({ 0,0,0,0 }));

	config = config.child("rect_previews").first_child();

	for (int i = Entity::entityType::TOWNHALL; i < Entity::entityType::WAR_HOUND; i++) {
		preview_rects->at(i).x = config.attribute((!isPlayer1) ? "rx" : "x").as_int(0);
		preview_rects->at(i).y = config.attribute((!isPlayer1) ? "ry" : "y").as_int(0);
		preview_rects->at(i).w = config.attribute("w").as_int(0);
		preview_rects->at(i).h = config.attribute("h").as_int(0);
		config = config.next_sibling();
	}
	return true;
}

bool Player::Start()
{

	time_iterator = number_of_troops = BuildingCost = BarracksCreated = TroopCost = 0;

	gold = 3500;
	gold_persecond = 0;

	SoldiersCreated = TankmansCreated = InfiltratorsCreated = EngineersCreated = WarHoundsCreated = Invulnerable_abilities = Rocket_abilities = Tank_abilities = 0;

	selected_texture = { 0,0, 100, 100 };

	UI_troop_type = Entity::entityType::SOLDIER;

	isBuilding = isDeploying = gold_added = isCasting = Y_pressed = isPaused = false;
	Soldier_Offensive = Tankman_Offensive = Engineer_Offensive = Infiltrator_Offensive = WarHound_Offensive = true;

	Y_pressed = true;
  
	currentTile = { 13,0 };
	
	return true;
}
void Player::RectangleSelection()
{
	pair<int, int> mouse_pos;
	int x, y;
	App->input->GetMousePosition(x, y);
	mouse_pos = App->render->ScreenToWorld(x, y);

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
		rectangle_origin.x = mouse_pos.first;
		rectangle_origin.y = mouse_pos.second;

	}

	else if (abs(mouse_pos.first - rectangle_origin.x) >= 5 && abs(mouse_pos.second - rectangle_origin.y) >= 5 && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
		// --- Rectangle size ---
		rectangle_origin.w = mouse_pos.first - rectangle_origin.x;
		rectangle_origin.h = mouse_pos.second - rectangle_origin.y;

		// --- Draw Rectangle ---
		//SDL_Rect SRect = { rectangle_origin.x, rectangle_origin.y, rectangle_origin.w, rectangle_origin.h };
		App->render->DrawQuad(rectangle_origin, 255, 255, 255, 255, false);

		// --- Once we get to the negative side of SRect numbers must be adjusted ---
		if (rectangle_origin.w < 0) {
			//SRect.x = mouse_pos.first;
			rectangle_origin.w *= -1;
		}
		if (rectangle_origin.h < 0) {
			//SRect.y = mouse_pos.second;
			rectangle_origin.h *= -1;
		}

		// --- Check for Units in the rectangle, select them ---

		App->move_manager->SelectEntities_inRect(rectangle_origin);

		//LOG("rect is x%i y%i w%i h%i", SRect.x, SRect.y, SRect.w, SRect.h);
	}

	//else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
	//	App->Mmanager->CreateGroup();

}

bool Player::DeployTroops(Entity::entityType type, int amount, pair<int,int> pos) {

	if (deploy_state == DeployState::START) {
		deploying_counter = 0;

		std::list<Troop*>::iterator entity = troops.begin();
		while (entity != troops.end())
		{
			(*entity)->isSelected = false;
			entity++;
		}
		deploy_state = DeployState::DEPLOYING;
	}
	else if (deploy_state == DeployState::DEPLOYING){
		if (deploying_counter >= amount) {
			deploy_state = DeployState::END;
		}
		else {
			App->input->GetMousePosition(pos.first, pos.second);
			pos = App->render->ScreenToWorld(pos.first, pos.second);
			collider.dimensions = { 1,1 };

			Troop* e;
			pos.first += deploying_counter * 9;
			e = (Troop*)App->entitymanager->AddEntity(isPlayer1, Entity::entityType::SOLDIER, pos, collider);
			e->state = NOT_DEPLOYED;
			e->isSelected = true;

			deploying_counter++;
		}
	}
	else if (deploy_state == DeployState::END) {
		deploying_counter = 0;
		isDeploying = false;
		groups.push_back(App->move_manager->CreateGroup(this));
		group++;
		return isDeploying;
	}

}

bool Player::Update(float dt)
{
	BROFILER_CATEGORY("Player Update", Profiler::Color::Black);

	if (inmune && App->scene->worldseconds == desired_second && App->scene->worldminutes == desired_min)
	{
		inmune = false;
	}

	if (!App->scene->endgame)
	{
		RectangleSelection();

		if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN) {
			std::list <Troop*>::const_iterator unit = troops.begin();
			while (unit != troops.end()) {
				if ((*unit)->isSelected) {
					(*unit)->health = -1;
				}
				unit++;
			}
		}

		//Preview all player1 entities with M
		if (App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN) 
		{
			isBuilding = !isBuilding;
			DoLogic(App->player1->Def_AOE_icon); //AOE
		}

		if (isBuilding && App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN) {
			isPlayer1 = false;
			type = (Entity::entityType)((curr++) % (int)Entity::entityType::TANKMAN);

		}
		if (App->input->GetKey(SDL_SCANCODE_G) == KEY_DOWN && (deploy_state == DeployState::END)) {

			deploy_state = DeployState::START;
		}

		//DeployTroops(Entity::entityType::SOLDIER, 9, { 0,0 });

		//--- Press X (Square) To SELECT BUILDINGS
		if (gamepad.Controller[BUTTON_X] == KEY_UP && currentUI == CURRENT_UI::CURR_MAIN)
		{
			building_selected = buildings.begin();

			while ((*building_selected)->type == Entity::entityType::WALLS)
				building_selected++;

			last_building = buildings.end();
			last_building--;
			currentUI = CURRENT_UI::CURR_SELECTING_BUILDING;
		}
		else if (gamepad.Controller[BUTTON_B] == KEY_UP && currentUI == CURRENT_UI::CURR_SELECTING_BUILDING)
		{
			building_selected._Ptr = nullptr;
			currentUI = CURRENT_UI::CURR_MAIN;
		}

		// DRAW QUAD on SELECTED BUILDING 
		if (currentUI == CURRENT_UI::CURR_SELECTING_BUILDING)
		{

			if (In_SelectBuilding->visible == false)
				In_SelectBuilding->visible = true;
			
			DrawBuildingCollider((*building_selected)->type, isPlayer1);
			
		}
		else
		{
			if(In_SelectBuilding!=nullptr && In_SelectBuilding->visible == true )
				In_SelectBuilding->visible = false;
		}

		
		// Button with focus changes state to HOVER 
		if (currentUI != CURRENT_UI::CURR_SELECTING_BUILDING && currentUI != CURRENT_UI::ENDGAME && currentUI != CURRENT_UI::CURR_WIN_SCREEN && gamepad.Controller[BUTTON_A] != KEY_REPEAT && focus._Ptr != nullptr)
		{
			(*focus)->state = UI_Element::State::HOVER;
		}

		// PAUSE
		if (gamepad.Controller[CONTROLLER_BUTTONS::START] == KEY_DOWN && App->scene->active)
		{
			if (!App->scene->pause)
			{
				App->scene->world_seconds.Stop();
				App->render->zoom = 1;
				Pause_UI->visible = true;
				isPaused = true;

				App->scene->pause = !App->scene->pause;

				if (currentUI != CURRENT_UI::NONE)
					(*focus)->state = UI_Element::State::IDLE;

				last_currentUI = currentUI;
				currentUI = CURRENT_UI::CURR_PAUSE;
				UpdateFocus(currentUI);
			}
			else if (App->scene->pause && isPaused && currentUI == CURRENT_UI::CURR_PAUSE)
			{
				App->scene->world_seconds.Start();
				App->render->zoom = 0.77;
				Pause_UI->visible = false;
				isPaused = false;

				App->scene->pause = !App->scene->pause;


				currentUI = last_currentUI;
				UpdateVisibility();
				UpdateFocus(currentUI);

			}


		}

		if (currentUI == CURRENT_UI::CURR_DEPLOY)
		{
			if (gamepad.Controller[BUTTON_X] == KEY_DOWN)
			{
				ChangeTroopsState();
			}
		}

		// From GENERAL UI to CREATE TROOPS UI (only for barracks)
		if (gamepad.Controller[BUTTON_X] == KEY_DOWN && currentUI == CURRENT_UI::CURR_GENERAL && (*building_selected)->type == Entity::entityType::BARRACKS )
		{
			currentUI = CURRENT_UI::CURR_CREATE_TROOPS;
			troop_icon->rect = { 662, 0, 85, 81 };
			UpdateVisibility();
			UI_troop_type = Entity::entityType::SOLDIER;
		}
		// From GENERAL UI to CREATE ABILITIES UI (only for command center)
		if (gamepad.Controller[BUTTON_X] == KEY_DOWN && currentUI == CURRENT_UI::CURR_GENERAL && (*building_selected)->type == Entity::entityType::COMMAND_CENTER)
		{
			currentUI = CURRENT_UI::CURR_CREATE_ABILITIES;
			UpdateVisibility();
			troop_icon->rect = { 576, 161, 85, 81 };
			UI_troop_type = 0;
			
		}

		

		//Creating TROOPS
		if (currentUI == CURRENT_UI::CURR_CREATE_TROOPS)
		{
			if (gamepad.Controller[LEFT] == KEY_DOWN)
			{
				UI_troop_type--;
				if (UI_troop_type < Entity::entityType::SOLDIER) //soldier
				{
					UI_troop_type = Entity::entityType::WAR_HOUND; // war_hound
				}
				Update_troop_image(UI_troop_type);

			}

			if (gamepad.Controller[RIGHT] == KEY_DOWN)
			{
				UI_troop_type++;
				if (UI_troop_type > Entity::entityType::WAR_HOUND) // war_hound
				{
					UI_troop_type = Entity::entityType::SOLDIER;//soldier
				}
				Update_troop_image(UI_troop_type);
			}

			if (gamepad.Controller[LB] == KEY_DOWN)
			{
				number_of_troops--;
				if (number_of_troops < 0)
				{
					number_of_troops = 0;
				}

			}

			if (gamepad.Controller[RB] == KEY_DOWN)
			{
				number_of_troops++;
				if (number_of_troops + (*building_selected)->TroopsCreated.size() > 10)
				{
					number_of_troops = 10 - (*building_selected)->TroopsCreated.size();
				}
				
			}

			if (UI_troop_type == Entity::entityType::SOLDIER)
			{
				TroopCost = 20 * number_of_troops;
			}
			else if (UI_troop_type == Entity::entityType::TANKMAN)
			{
				TroopCost = 80 * number_of_troops;
			}
			else if (UI_troop_type == Entity::entityType::INFILTRATOR)
			{
				TroopCost = 100 * number_of_troops;
			}
			else if (UI_troop_type == Entity::entityType::ENGINEER)
			{
				TroopCost = 25 * number_of_troops;
			}
			else if (UI_troop_type == Entity::entityType::WAR_HOUND)
			{
				TroopCost = 50 * number_of_troops;
			}

			if (gamepad.Controller[BUTTON_A] == KEY_UP && gold >= TroopCost)
			{
				CreateTroop(UI_troop_type, number_of_troops);
				Update_troop_image(UI_troop_type);
				GotoPrevWindows(currentUI);
				gold -= TroopCost;
			}

		}

		//Creating ABILITIES
		if (currentUI == CURRENT_UI::CURR_CREATE_ABILITIES)
		{
			if (gamepad.Controller[LEFT] == KEY_DOWN)
			{
				if (UI_troop_type == ABILITIES::INVULNERABLE) //soldier
				{
					UI_troop_type = ABILITIES::ROCKET; // war_hound
				}
				else
					UI_troop_type--;

				Update_troop_image(UI_troop_type);

			}

			if (gamepad.Controller[RIGHT] == KEY_DOWN)
			{
				UI_troop_type++;
				if (UI_troop_type > ABILITIES::ROCKET) // war_hound
				{
					UI_troop_type = ABILITIES::INVULNERABLE;//soldier
				}
				Update_troop_image(UI_troop_type);
			}

			if (gamepad.Controller[LB] == KEY_DOWN)
			{
				number_of_troops--;
				if (number_of_troops < 0)
				{
					number_of_troops = 0;
				}

			}

			if (gamepad.Controller[RB] == KEY_DOWN)
			{
				number_of_troops++;
				if (number_of_troops > 5)
				{
					number_of_troops = 5;
				}
			}

			if (UI_troop_type == ABILITIES::INVULNERABLE)
			{
				TroopCost = 2000 * number_of_troops;
			}
			else if (UI_troop_type == ABILITIES::ROCKET)
			{
				TroopCost = 3000 * number_of_troops;
			}
			else if (UI_troop_type == ABILITIES::TANK)
			{
				TroopCost = 3000 * number_of_troops;
			}

			if (gamepad.Controller[BUTTON_A] == KEY_UP && gold >= TroopCost)
			{
				CreateAbility(UI_troop_type, number_of_troops);
				GotoPrevWindows(currentUI);
				gold -= TroopCost;
			}

		}


		// Button A to clcik a button
		if (gamepad.Controller[BUTTON_A] == KEY_DOWN && currentUI != CURRENT_UI::NONE && currentUI != CURRENT_UI::CURR_SELECTING_BUILDING && currentUI != CURRENT_UI::CURR_PAUSE_SETTINGS && currentUI != CURRENT_UI::CURR_CREATE_TROOPS)
		{
			if (currentUI != CURRENT_UI::CURR_BUILD && currentUI != CURRENT_UI::CURR_DEPLOY && currentUI != CURRENT_UI::CURR_CAST)
				(*focus)->state = UI_Element::State::LOGIC;
		}

		// Do button action
		if (gamepad.Controller[BUTTON_A] == KEY_UP && currentUI != CURRENT_UI::NONE && currentUI != CURRENT_UI::CURR_SELECTING_BUILDING && currentUI != CURRENT_UI::CURR_PAUSE_SETTINGS && currentUI != CURRENT_UI::CURR_CREATE_TROOPS)
		{
			if (App->scene->pause && isPaused == true)
			{
				(*focus)->state = UI_Element::State::IDLE;
				DoLogic((*focus));
				UpdateFocus(currentUI);
			}
			else if (!App->scene->pause)
			{
				if (!isBuilding)
					(*focus)->state = UI_Element::State::IDLE;
				if (App->scene->active)
					DoLogic((*focus));
				else
					App->main_menu->DoLogic((*focus));

				if ((*focus) == Build_icon || (*focus) == Deploy_icon || (*focus) == Cast_icon)
					UpdateFocus(currentUI);
			}

		}

		// --TEST-- GENERAL UI (menu of selected building)
		if (gamepad.Controller[BUTTON_A] == KEY_UP && currentUI == CURRENT_UI::CURR_SELECTING_BUILDING && App->scene->active)
		{
			currentUI = CURRENT_UI::CURR_GENERAL;
			UpdateVisibility();
			UpdateFocus(currentUI);
		}

		if (currentUI == CURRENT_UI::CURR_GENERAL)
		{
			UpdateGeneralUI((*building_selected));
		}

		if (currentUI == CURRENT_UI::CURR_BUILD)
		{
			if ((*focus) == Def_AOE_icon)
			{
				BuildingCost = 500;
			}
			else if ((*focus) == Def_Target_icon)
			{
				BuildingCost = 800;
			}
			else if ((*focus) == Mines_icon)
			{
				BuildingCost = 2000;
			}
			else if ((*focus) == Barracks_icon)
			{
				BuildingCost = 1000;
			}

		}

		// Go back
		if (gamepad.Controller[BUTTON_B] == KEY_DOWN && currentUI != CURRENT_UI::CURR_MAIN)
		{
			if(focus._Ptr != nullptr)
				(*focus)->state = UI_Element::State::IDLE;

			if (App->scene->pause && isPaused && currentUI == CURRENT_UI::CURR_PAUSE)
			{
				App->scene->world_seconds.Start();
				App->render->zoom = 0.77;
				Pause_UI->visible = false;
				isPaused = false;

				App->scene->pause = !App->scene->pause;

				currentUI = last_currentUI;
				UpdateVisibility();
				UpdateFocus(currentUI);
			}
			else if (App->scene->pause && isPaused && currentUI != CURRENT_UI::CURR_PAUSE)
			{
				GotoPrevWindows(currentUI);
				UpdateFocus(currentUI);
			}
			else if (!App->scene->pause)
			{
				if (currentUI == CURR_BUILD || currentUI == CURR_DEPLOY)
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
		}


		//Change the side images from the menus
		if (App->scene->active)
		{
			if (Y_pressed == false)
			{
				Y_to_Main->visible = true;
				Y_to_Main2->visible = true;
				RB_img->visible = false;
				LB_img->visible = false;
			}
			else if (Y_pressed == true)
			{
				Y_to_Main->visible = false;
				Y_to_Main2->visible = false;
				RB_img->visible = true;
				LB_img->visible = true;
			}

			if (currentUI != CURRENT_UI::CURR_GENERAL && Create_troops!=nullptr)
				Create_troops->visible = false;

			if (currentUI != CURRENT_UI::CURR_GENERAL && Create_abilities != nullptr)
				Create_abilities->visible = false;

			if(currentUI == CURR_MAIN)
				SelectBuilding->visible = true;
			else
				SelectBuilding->visible = false;
		}


		// Travel through the different buttons
		if ((gamepad.Controller[RB] == KEY_DOWN || gamepad.Controller[RIGHT] == KEY_DOWN || gamepad.Controller[JOY_RIGHT] == KEY_DOWN) &&
			currentUI != CURRENT_UI::NONE && currentUI != CURRENT_UI::CURR_CREATE_TROOPS && currentUI != CURRENT_UI::CURR_CREATE_ABILITIES && 
			gamepad.Controller[BUTTON_A] != KEY_REPEAT && isBuilding == false && !App->scene->pause && App->scene->active)
		{
			App->audio->PlayFx(CHANGE_FOCUS);
			if (currentUI != CURRENT_UI::CURR_SELECTING_BUILDING)
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
			else
			{
				if (building_selected == last_building)
				{
					building_selected = buildings.begin();

				}
				else
				{
					building_selected++;
										
				}

				if ((*building_selected)->type == Entity::entityType::WALLS)
				{
					while ((*building_selected)->type == Entity::entityType::WALLS)
					{
						building_selected++;
						if (building_selected == last_building)
						{
							building_selected = buildings.begin();

						}
					}
				}
			}
			

		}
		// Travel through the different buttons
		if ((gamepad.Controller[LB] == KEY_DOWN || gamepad.Controller[LEFT] == KEY_DOWN || gamepad.Controller[JOY_LEFT] == KEY_DOWN) &&
			currentUI != CURRENT_UI::NONE && currentUI != CURRENT_UI::CURR_CREATE_TROOPS && currentUI != CURRENT_UI::CURR_CREATE_ABILITIES && 
			gamepad.Controller[BUTTON_A] != KEY_REPEAT && isBuilding == false && !App->scene->pause && App->scene->active)
		{
			App->audio->PlayFx(CHANGE_FOCUS);
			if (currentUI != CURRENT_UI::CURR_SELECTING_BUILDING)
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
			else
			{
				if (building_selected == buildings.begin())
				{
					building_selected = last_building;
				}
				else
				{
					building_selected--;
					if ((*building_selected)->type == Entity::entityType::WALLS)
					{
						while ((*building_selected)->type == Entity::entityType::WALLS)
						{
							building_selected--;
						}
					}
				}
			}
			

		}

		//Travel through buttons with DPAD in pause and mainmenu
		if (App->main_menu->active || App->scene->pause)
		{
			if ((gamepad.Controller[UP] == KEY_DOWN || gamepad.Controller[JOY_UP] == KEY_DOWN) && currentUI != CURRENT_UI::NONE && gamepad.Controller[BUTTON_A] != KEY_REPEAT)
			{
				App->audio->PlayFx(CHANGE_FOCUS);
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

			if ((gamepad.Controller[DOWN] == KEY_DOWN || gamepad.Controller[JOY_DOWN] == KEY_DOWN) && currentUI != CURRENT_UI::NONE && gamepad.Controller[BUTTON_A] != KEY_REPEAT)
			{
				App->audio->PlayFx(CHANGE_FOCUS);
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
		}

		// Increase or decrease volume
		if ((gamepad.Controller[RIGHT] == KEY_DOWN || gamepad.Controller[JOY_RIGHT] == KEY_DOWN) && ( currentUI == CURRENT_UI::CURR_PAUSE_SETTINGS || currentUI == CURRENT_UI::CURR_MM_SETTINGS))
		{
			App->audio->PlayFx(SLIDER_UP);
			if (((*focus) == Music_Settings || (*focus) == App->main_menu->Music_Settings) && App->audio->musicVolume < 100)
			{
				App->audio->musicVolume += 10;
				App->audio->SetMusicVolume();


			}
			else if (((*focus) == FX_Settings || (*focus) == App->main_menu->FX_Settings) && App->audio->sfxVolume < 100)
			{
				App->audio->sfxVolume += 10;

				App->audio->SetSfxVolume();
			}
		}
		else if ((gamepad.Controller[LEFT] == KEY_DOWN || gamepad.Controller[JOY_LEFT] == KEY_DOWN) && (currentUI == CURRENT_UI::CURR_PAUSE_SETTINGS || currentUI == CURRENT_UI::CURR_MM_SETTINGS))
		{
			App->audio->PlayFx(SLIDER_DOWN);
			if (((*focus) == Music_Settings || (*focus) == App->main_menu->Music_Settings) && App->audio->musicVolume > 0)
			{
				App->audio->musicVolume -= 10;

				App->audio->SetMusicVolume();
			}
			else if (((*focus) == FX_Settings || (*focus) == App->main_menu->FX_Settings) && App->audio->sfxVolume > 0)
			{
				App->audio->sfxVolume -= 10;

				App->audio->SetSfxVolume();
			}
		}

		// Just to test the LiveBar. Can only be used in GodMode (Press F10)
		if (!App->scene->pause && gamepad.Controller[UP] == KEY_DOWN && App->scene->active && App->scene->godmode)
		{
			Townhall->health -= 1000;
			if (Townhall->health < 0)
			{
				Townhall->health = 0;
			}

		}
	}
	
	if (App->scene->endgame && gamepad.Controller[BUTTON_A] == KEY_DOWN)
	{
		App->scene->ResetGame();
	}

	//--- Building ---------------------
	if (isBuilding && !App->scene->pause)
	{
		//--- Movement
		if (gamepad.Controller[JOY_UP] == KEY_REPEAT || gamepad.Controller[UP] == KEY_DOWN ||
			App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_B) == KEY_REPEAT)
		{
			currentTile.first--;
			currentTile.second--;
		}
		else if (gamepad.Controller[JOY_DOWN] == KEY_REPEAT || gamepad.Controller[DOWN] == KEY_DOWN ||
			App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_G) == KEY_REPEAT)
		{
			currentTile.first++;
			currentTile.second++;
		}

		if (gamepad.Controller[JOY_RIGHT] == KEY_REPEAT || gamepad.Controller[RIGHT] == KEY_DOWN ||
			App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT)
		{
			currentTile.first++;
			currentTile.second--;
		}
		else if (gamepad.Controller[JOY_LEFT] == KEY_REPEAT || gamepad.Controller[LEFT] == KEY_DOWN ||
			App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_H) == KEY_REPEAT)
		{
			currentTile.first--;
			currentTile.second++;
		}

		//--- Limits
		//need to modify, when player1 is building we must chehk its width
		if (isPlayer1 == true)
		{
			if (currentTile.first <= x_limits_player1.first) //left limit
			{
				currentTile.first = x_limits_player1.first;
			}
			else if (currentTile.first + collider.dimensions.first >= x_limits_player1.second) //right limit
			{
				currentTile.first = x_limits_player1.second - collider.dimensions.first;
			}

			if (currentTile.second < y_limits_player1.first) //up limit
			{
				currentTile.second = y_limits_player1.first;
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
				currentTile.first = x_limits_player2.second - collider.dimensions.first;
			}
			else if (currentTile.first <= x_limits_player2.first) //left limit
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


		//--- Press A
		App->map->debug = true;
		if (CheckBuildingPos() == true) // Can build
		{
			//Preview entity to spawn
			pair<int, int> pos;
			App->input->GetMousePosition(pos.first, pos.second);
			pos = App->render->ScreenToWorld(pos.first, pos.second);
			//pos.first--;

			if (type == Entity::entityType::TOWNHALL)
			{
				App->render->Blit(App->entitymanager->entitiesTextures[type], collider.tiles[0].first, collider.tiles[0].second, &(preview_rects->at(type)));
			}
			else if (type == Entity::entityType::BARRACKS)
			{
				//157 x 136
				App->render->Blit(App->entitymanager->entitiesTextures[type], collider.tiles[0].first-30, collider.tiles[0].second-110, &(preview_rects->at(type)));
			}
			else if (type == Entity::entityType::MINES)
			{
				//190 x 140
				App->render->Blit(App->entitymanager->entitiesTextures[type], collider.tiles[0].first-80, collider.tiles[0].second-70, &(preview_rects->at(type)));
			}
			else if (type == Entity::entityType::DEFENSE_AOE)
			{
				//92 x 92
				App->render->Blit(App->entitymanager->entitiesTextures[type], collider.tiles[0].first, collider.tiles[0].second-70, &(preview_rects->at(type)));
			}
			else if (type == Entity::entityType::DEFENSE_TARGET)
			{
				App->render->Blit(App->entitymanager->entitiesTextures[type], collider.tiles[0].first, collider.tiles[0].second, &(preview_rects->at(type)));
			}
			else if (type == Entity::entityType::MAIN_DEFENSE)
			{
				App->render->Blit(App->entitymanager->entitiesTextures[type], collider.tiles[0].first, collider.tiles[0].second, &(preview_rects->at(type)));
			}
			/*else 
			{
				App->render->Blit(App->entitymanager->entitiesTextures[type], collider.tiles[0].first, collider.tiles[0].second, &(preview_rects->at(type)));
			}*/

			if (gamepad.Controller[LB] == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN) //previous building
			{
				if (number <= 1)
					number = 4;
				else
					number--;

				ChangeBuilding(number);
			}
			else if (gamepad.Controller[RB] == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN) //next building
			{
				if (number >= 4)
					number = 1;
				else
					number++;

				ChangeBuilding(number);
			}

			if (gamepad.Controller[BUTTON_A] == KEY_DOWN || App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
			{
				
				if (gold >= CheckCost(type))
				{
					App->entitymanager->AddEntity(isPlayer1, type, { collider.tiles[0].first /*- offset.first*/, collider.tiles[0].second /*- offset.second*/ }, collider);
					if (type == Entity::entityType::BARRACKS)
					{
						BarracksCreated++;
					}

				}
				else
					App->audio->PlayFx(WRONG);

				isBuilding = false;
							
			}

		}
		else
		{
			if (gamepad.Controller[BUTTON_A] == KEY_DOWN || App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
			{
				if (type >= Entity::entityType::TOWNHALL && type <= Entity::entityType::BARRACKS) //if building
				{
					//buildings.pop_back();

				}
				else if (type > Entity::entityType::BARRACKS) //if troops
				{
					//troops.pop_back();
				}
				isBuilding = false;
				
				App->audio->PlayFx(WRONG);
				//play fx (error);
			}
		}
		if (type > Entity::entityType::BARRACKS) //if troops
		{
			if (X_spawn->visible == false)
			{
				X_spawn->visible = true;
			}

			if (A_spawn->visible == false)
			{
				A_spawn->visible = true;
			}
		}
		
			

	}
	else
	{
		if (X_spawn!=nullptr && X_spawn->visible == true)
			X_spawn->visible = false;

		if (A_spawn != nullptr && A_spawn->visible == true)
			A_spawn->visible = false;
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

	entities.clear();

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
		pos = App->render->ScreenToWorld(pos.first, pos.second);

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

	// Compare tiles with walkability map
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

Collider Player::GetCollider(pair<int, int> dimensions, pair<int, int> topTile_pos)
{
	int cont = 0;
	pair<int, int> real_pos;

	Collider new_collider;
	new_collider.dimensions = dimensions;

	for (int i = 0; i < new_collider.dimensions.first; i++)
	{
		for (int j = 0; j < new_collider.dimensions.second; j++)
		{
			real_pos = App->map->MapToWorld(topTile_pos.first, topTile_pos.second);
			new_collider.tiles.push_back(real_pos);
			topTile_pos.second++;
			cont = j;
		}
		topTile_pos.first++;
		topTile_pos.second -= cont + 1;
	}
	return new_collider;
}


void Player::UpdateWalkabilityMap(char cell_type, Collider collider) //update walkable tiles
{
	for (int i = 0; i < collider.tiles.size(); ++i)
	{
		pair <int, int> pos = App->map->WorldToMap(collider.tiles[i].first, collider.tiles[i].second);
		if (App->pathfinding->GetTileAt(pos) != cell_type)
		{
			App->pathfinding->ChangeWalkability(pos, cell_type);
			// Debug drawing
			App->map->walkability_layer->Set(pos.first, pos.second, 1);
		}
	}
}

int Player::CheckCost(Entity::entityType type)
{
	//
	if (type == Entity::entityType::BARRACKS)
		return 1000;

	else if (type == Entity::entityType::DEFENSE_AOE)
		return 500;

	else if (type == Entity::entityType::MINES)
		return 2000;

	else if (type == Entity::entityType::MAIN_DEFENSE) // Torreta single target (esta al reves?)
		return 800;

	else
		return 0;

}

int Player::GoldKill(Entity* entity)
{
	if (entity->type == Entity::entityType::SOLDIER)
		return 100;

	else
		return 0;
}

void Player::ChangeBuilding(int num)
{
	if (isPlayer1 == true)
	{
		if (num == 1)
		{
			DoLogic(App->player1->Def_AOE_icon); //AOE
		}
		else if (num == 2)
		{
			DoLogic(App->player1->Def_Target_icon); //Target
		}
		else if (num == 3)
		{
			DoLogic(App->player1->Mines_icon); //Mine
		}
		else if (num == 4)
		{
			DoLogic(App->player1->Barracks_icon); //Barracks
		}
	}
	else if (isPlayer1 == false)
	{
		if (num == 1)
		{
			DoLogic(App->player2->Def_AOE_icon); //AOE
		}
		else if (num == 2)
		{
			DoLogic(App->player2->Def_Target_icon); //Target
		}
		else if (num == 3)
		{
			DoLogic(App->player2->Mines_icon); //Mine
		}
		else if (num == 4)
		{
			DoLogic(App->player2->Barracks_icon); //Barracks
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

	case::Player::CURRENT_UI::CURR_MM_SETTINGS:
		focus = App->main_menu->MM_Settings_UI->children.begin();
		last_element = App->main_menu->MM_Settings_UI->children.end();
		last_element--;
		break;

	case::Player::CURRENT_UI::CURR_MM_CREDITS:
		focus = App->main_menu->Credits_UI->children.begin();
		last_element = App->main_menu->Credits_UI->children.end();
		last_element--;

		focus = last_element;
		break;

	case::Player::CURRENT_UI::CURR_MAIN:
		focus = Main_UI->children.begin();
		last_element = Main_UI->children.end();
		last_element--;
		break;

	case::Player::CURRENT_UI::CURR_GENERAL:
		focus = General_UI->children.begin();
		last_element = General_UI->children.end();
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
      
	case::Player::CURRENT_UI::CURR_PAUSE:
		focus = Pause_UI->children.begin();
		last_element = Pause_UI->children.end();
		last_element--;
		break;

	case::Player::CURRENT_UI::CURR_PAUSE_SETTINGS:
		focus = Settings_UI->children.begin();
		last_element = Settings_UI->children.end();
		last_element--;
		break;

	case::Player::CURRENT_UI::CURR_PAUSE_ABORT:
		focus = Abort_UI->children.begin();
		last_element = Abort_UI->children.end();
		last_element--;
		break;
	
	}
}


void Player::GotoPrevWindows(uint data)
{
	switch (data)
	{
	case Player::CURRENT_UI::CURR_MM_SETTINGS:
		currentUI = CURRENT_UI::CURR_MAIN_MENU;
		UpdateVisibility();
		break;

	case Player::CURRENT_UI::CURR_MM_CREDITS:
		currentUI = CURRENT_UI::CURR_MAIN_MENU;
		App->main_menu->license_texture->visible = false;
		App->main_menu->authors_texture->visible = false;
		UpdateVisibility();
		break;

	case Player::CURRENT_UI::CURR_MAIN:
		
		break;

	case Player::CURRENT_UI::CURR_GENERAL:
		currentUI = CURRENT_UI::CURR_MAIN;
		UpdateVisibility();
		break;

	case Player::CURRENT_UI::CURR_CREATE_TROOPS:
		currentUI = CURRENT_UI::CURR_GENERAL;
		number_of_troops = 0;
		UpdateVisibility();
		break;

	case Player::CURRENT_UI::CURR_CREATE_ABILITIES:
		currentUI = CURRENT_UI::CURR_GENERAL;
		number_of_troops = 0;
		UpdateVisibility();
		break;

	case Player::CURRENT_UI::CURR_BUILD:
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

	case Player::CURRENT_UI::CURR_PAUSE:
		currentUI = last_currentUI;
		UpdateVisibility();
		break;

	case Player::CURRENT_UI::CURR_PAUSE_SETTINGS:
		currentUI = CURR_PAUSE;
		UpdateVisibility();
		break;

	case Player::CURRENT_UI::CURR_PAUSE_ABORT:
		currentUI = CURR_PAUSE;
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

	case::Player::CURRENT_UI::CURR_MM_SETTINGS:
		return App->main_menu->MM_Settings_UI;

	case::Player::CURRENT_UI::CURR_MM_CREDITS:
		return App->main_menu->Credits_UI;

	case::Player::CURRENT_UI::CURR_MAIN:
		return Main_UI;

	case::Player::CURRENT_UI::CURR_BUILD:
		return Build_UI;

	case::Player::CURRENT_UI::CURR_CAST:
		return Cast_UI;

	case::Player::CURRENT_UI::CURR_DEPLOY:
		return Deploy_UI;

	case::Player::CURRENT_UI::CURR_PAUSE:
		return Pause_UI;

	case::Player::CURRENT_UI::CURR_PAUSE_SETTINGS:
		return Settings_UI;

	case::Player::CURRENT_UI::CURR_PAUSE_ABORT:
		return Abort_UI;

	case::Player::CURRENT_UI::CURR_GENERAL:
		return General_UI;

	}
}

void Player::UpdateVisibility() // Update GUI Visibility
{
	switch (currentUI)
	{
	case::Player::CURRENT_UI::NONE:
		Main_UI->visible = true;
		Build_UI->visible = false;
		Deploy_UI->visible = false;
		Cast_UI->visible = false;
		Pause_UI->visible = false;
		Abort_UI->visible = false;
		Settings_UI->visible = false;
		win_screen->visible = false;
		General_UI->visible = false;
		Create_Troops_UI->visible = false;
		break;
	case::Player::CURRENT_UI::CURR_MAIN:
		Main_UI->visible = true;
		Build_UI->visible = false;
		Deploy_UI->visible = false;
		Cast_UI->visible = false;
		Pause_UI->visible = false;
		Settings_UI->visible = false;
		Abort_UI->visible = false;
		win_screen->visible = false;
		General_UI->visible = false;
		Create_Troops_UI->visible = false;
		break;

	case::Player::CURRENT_UI::CURR_BUILD:
		Main_UI->visible = false;
		Build_UI->visible = true;
		Deploy_UI->visible = false;
		Cast_UI->visible = false;
		Pause_UI->visible = false;
		Settings_UI->visible = false;
		Abort_UI->visible = false;
		win_screen->visible = false;
		General_UI->visible = false;
		Create_Troops_UI->visible = false;
		break;

	case::Player::CURRENT_UI::CURR_DEPLOY:
		Main_UI->visible = false;
		Build_UI->visible = false;
		Deploy_UI->visible = true;
		Cast_UI->visible = false;
		Pause_UI->visible = false;
		Settings_UI->visible = false;
		Abort_UI->visible = false;
		win_screen->visible = false;
		General_UI->visible = false;
		Create_Troops_UI->visible = false;
		break;

	case::Player::CURRENT_UI::CURR_CAST:
		Main_UI->visible = false;
		Build_UI->visible = false;
		Deploy_UI->visible = false;
		Cast_UI->visible = true;
		Pause_UI->visible = false;
		Settings_UI->visible = false;
		Abort_UI->visible = false;
		win_screen->visible = false;
		General_UI->visible = false;
		Create_Troops_UI->visible = false;
		break;

	case::Player::CURRENT_UI::CURR_GENERAL:
		Main_UI->visible = false;
		Build_UI->visible = false;
		Deploy_UI->visible = false;
		Cast_UI->visible = false;
		Pause_UI->visible = false;
		Abort_UI->visible = false;
		Settings_UI->visible = false;
		win_screen->visible = false;
		General_UI->visible = true;
		Create_Troops_UI->visible = false;

		break;

	case::Player::CURRENT_UI::CURR_CREATE_TROOPS:

		Main_UI->visible = false;
		Build_UI->visible = false;
		Deploy_UI->visible = false;
		Cast_UI->visible = false;
		Pause_UI->visible = false;
		Abort_UI->visible = false;
		Settings_UI->visible = false;
		win_screen->visible = false;
		General_UI->visible = false;
		Create_Troops_UI->visible = true;

		break;

	case::Player::CURRENT_UI::CURR_CREATE_ABILITIES:

		Main_UI->visible = false;
		Build_UI->visible = false;
		Deploy_UI->visible = false;
		Cast_UI->visible = false;
		Pause_UI->visible = false;
		Abort_UI->visible = false;
		Settings_UI->visible = false;
		win_screen->visible = false;
		General_UI->visible = false;
		Create_Troops_UI->visible = true;

		break;

	case::Player::CURRENT_UI::CURR_PAUSE:
		Main_UI->visible = false;
		Build_UI->visible = false;
		Deploy_UI->visible = false;
		Cast_UI->visible = false;
		Pause_UI->visible = true;
		Settings_UI->visible = false;
		Abort_UI->visible = false;
		win_screen->visible = false;
		General_UI->visible = false;
		Create_Troops_UI->visible = false;
		break;

	case::Player::CURRENT_UI::CURR_PAUSE_SETTINGS:
		Main_UI->visible = false;
		Build_UI->visible = false;
		Deploy_UI->visible = false;
		Cast_UI->visible = false;
		Pause_UI->visible = false;
		Settings_UI->visible = true;
		Abort_UI->visible = false;
		win_screen->visible = false;
		General_UI->visible = false;
		Create_Troops_UI->visible = false;
		break;

	case::Player::CURRENT_UI::CURR_PAUSE_ABORT:
		Main_UI->visible = false;
		Build_UI->visible = false;
		Deploy_UI->visible = false;
		Cast_UI->visible = false;
		Pause_UI->visible = false;
		Settings_UI->visible = false;
		Abort_UI->visible = true;
		win_screen->visible = false;
		General_UI->visible = false;
		Create_Troops_UI->visible = false;
		break;
	case::Player::CURRENT_UI::CURR_WIN_SCREEN:
		Main_UI->visible = false;
		Build_UI->visible = false;
		Deploy_UI->visible = false;
		Cast_UI->visible = false;
		Pause_UI->visible = false;
		Settings_UI->visible = false;
		Abort_UI->visible = false;
		win_screen->visible = true;
		Gold_UI->visible = false;
		General_UI->visible = false;
		Create_Troops_UI->visible = false;
		Create_abilities->visible = false;
		Create_troops->visible = false;
		Y_to_Main2->visible = false;
		Y_to_Main->visible = false;
		LB_img->visible = false;
		RB_img->visible = false;
		SelectBuilding->visible = false;
		In_SelectBuilding->visible = false;
		draw_screen->visible = false;
		break;
	case::Player::CURRENT_UI::ENDGAME: //Dont show the other player win screen
		Main_UI->visible = false;
		Build_UI->visible = false;
		Deploy_UI->visible = false;
		Cast_UI->visible = false;
		Pause_UI->visible = false;
		Settings_UI->visible = false;
		Abort_UI->visible = false;
		win_screen->visible = false;
		Gold_UI->visible = false;
		App->scene->ui_timer->visible = false;
		General_UI->visible = false;
		Create_Troops_UI->visible = false;
		Create_troops->visible = false;
		Create_abilities->visible = false;
		Y_to_Main2->visible = false;
		Y_to_Main->visible = false;
		LB_img->visible = false;
		RB_img->visible = false;
		SelectBuilding->visible = false;
		In_SelectBuilding->visible = false;
		draw_screen->visible = false;

	case::Player::CURRENT_UI::DRAW: //Dont show the other player win screen
		Main_UI->visible = false;
		Build_UI->visible = false;
		Deploy_UI->visible = false;
		Cast_UI->visible = false;
		Pause_UI->visible = false;
		Settings_UI->visible = false;
		Abort_UI->visible = false;
		win_screen->visible = false;
		Gold_UI->visible = false;
		App->scene->ui_timer->visible = false;
		General_UI->visible = false;
		Create_Troops_UI->visible = false;
		Create_troops->visible = false;
		Create_abilities->visible = false;
		Y_to_Main2->visible = false;
		Y_to_Main->visible = false;
		LB_img->visible = false;
		RB_img->visible = false;
		SelectBuilding->visible = false;
		In_SelectBuilding->visible = false;
		draw_screen->visible = true;
		break;

	case::Player::CURRENT_UI::CURR_MAIN_MENU:
		App->main_menu->menu_background->visible = true;
		App->main_menu->MM_Settings_UI->visible = false;
		App->main_menu->Credits_UI->visible = false;
		break;

	case::Player::CURRENT_UI::CURR_MM_SETTINGS:
		App->main_menu->menu_background->visible = false;
		App->main_menu->MM_Settings_UI->visible = true;
		App->main_menu->Credits_UI->visible = false;
		break;

	case::Player::CURRENT_UI::CURR_MM_CREDITS:
		App->main_menu->menu_background->visible = false;
		App->main_menu->MM_Settings_UI->visible = false;
		App->main_menu->Credits_UI->visible = true;
		break;

	}
	App->gui->UpdateChildren();
}

void Player::DoLogic(UI_Element* data)
{
		

	switch (data->action)
	{

	/*case::UI_Element::Action::NEW_GAME:
		App->scenechange->ContinueGame = true;
		App->scenechange->SwitchScene(App->scene, App->main_menu);
		menu_background->visible = false;
		break;*/

	case::UI_Element::Action::ACT_GOTO_MAIN:
		currentUI = CURR_MAIN;
		UpdateVisibility();
		break;

	case::UI_Element::Action::ACT_GOTO_BUILD:
		currentUI = CURR_BUILD;
		UpdateVisibility();
		App->audio->PlayFx(INGAME_CLICK);
		break;

	case::UI_Element::Action::ACT_GOTO_DEPLOY:
		currentUI = CURR_DEPLOY;
		UpdateVisibility();
		App->audio->PlayFx(INGAME_CLICK);
		break;

	case::UI_Element::Action::ACT_GOTO_CAST:
		currentUI = CURR_CAST;
		UpdateVisibility();
		App->audio->PlayFx(INGAME_CLICK);
		break;

	case::UI_Element::Action::ACT_BUILD_AOE:
		isBuilding = true;
		type = Entity::entityType::DEFENSE_AOE;
		collider.dimensions = { 2,2 };
		offset = { 20,30 };
		number = 1;
		break;

	case::UI_Element::Action::ACT_BUILD_TARGET:
		isBuilding = true;
		type = Entity::entityType::MAIN_DEFENSE;
		collider.dimensions = { 2,2 };
		offset = { 10 , 0 };
		number = 2;
		break;

	case::UI_Element::Action::ACT_BUILD_MINE:
		isBuilding = true;
		type = Entity::entityType::MINES;
		collider.dimensions = { 4,4 };
		offset = { 60, 30 };
		number = 3;
		break;

	case::UI_Element::Action::ACT_BUILD_BARRACKS:
		if (BarracksCreated < 3)
		{
			isBuilding = true;
			type = Entity::entityType::BARRACKS;
			collider.dimensions = { 3,4 };
			offset = { 40 , 50 };
			number = 4;
			/*collider.dimensions = { 3,3 };
			offset = { 0, 0 }; */

		}
		break;

	case::UI_Element::Action::ACT_DEPLOY_SOLDIER:

		/*{
			App->audio->PlayFx(INGAME_CLICK);
			isBuilding = true;
			type = Entity::entityType::SOLDIER;
			collider.dimensions = { 1,1 };
		}*/
		Soldier_Offensive = !Soldier_Offensive;

		if (Soldier_Offensive)
		{
			Soldier_Deff->rect = { 1219,98,20,21 };
			Soldier_Off->rect = { 1195,123,18,17 };
		}
		else
		{
			Soldier_Deff->rect = { 1195,98,20,21 };
			Soldier_Off->rect = { 1220,123,18,17 };
		}
		break;

	case::UI_Element::Action::ACT_DEPLOY_TANKMAN:
		/*{
			App->audio->PlayFx(INGAME_CLICK);
			isBuilding = true;
			type = Entity::entityType::TANKMAN;
			collider.dimensions = { 1,1 };
		}*/
		Tankman_Offensive = !Tankman_Offensive;
		if (Tankman_Offensive)
		{
			Tankman_Deff->rect = { 1219,98,20,21 };
			Tankman_Off->rect = { 1195,123,18,17 };
		}
		else
		{
			Tankman_Deff->rect = { 1195,98,20,21 };
			Tankman_Off->rect = { 1220,123,18,17 };

		}
		break;

	case::UI_Element::Action::ACT_DEPLOY_INFILTRATOR:

		//
		//if (WarHoundsCreated > 0)
	/*	{
			App->audio->PlayFx(INGAME_CLICK);
			isBuilding = true;
			type = Entity::entityType::INFILTRATOR;
			collider.dimensions = { 1,1 };
		}*/

		Infiltrator_Offensive = !Infiltrator_Offensive;
		if (Infiltrator_Offensive)
		{
			Infiltrator_Deff->rect = { 1219,98,20,21 };
			Infiltrator_Off->rect = { 1195,123,18,17 };
		}
		else
		{
			Infiltrator_Deff->rect = { 1195,98,20,21 };
			Infiltrator_Off->rect = { 1220,123,18,17 };
		}
		
		//App->audio->PlayFx(WRONG);

		break;

	case::UI_Element::Action::ACT_DEPLOY_ENGINEER:
		//
		//if (WarHoundsCreated > 0)
		Engineer_Offensive = !Engineer_Offensive;
		if (Engineer_Offensive)
		{
			Engineer_Deff->rect = { 1219,98,20,21 };
			Engineer_Off->rect = { 1195,123,18,17 };
		}
		else
		{
			Engineer_Deff->rect = { 1195,98,20,21 };
			Engineer_Off->rect = { 1220,123,18,17 };
		}
	
		/*{
			App->audio->PlayFx(INGAME_CLICK);
			isBuilding = true;
			type = Entity::entityType::ENGINEER;
			collider.dimensions = { 1,1 };
		}*/

	//App->audio->PlayFx(WRONG);

	
		break;

	case::UI_Element::Action::ACT_DEPLOY_WARHOUND:

		//
		//if (WarHoundsCreated > 0)
		WarHound_Offensive = !WarHound_Offensive;
		if (WarHound_Offensive)
		{
			WarHound_Deff->rect = { 1219,98,20,21 };
			WarHound_Off->rect = { 1195,123,18,17 };
		}
		else
		{
			WarHound_Deff->rect = { 1195,98,20,21 };
			WarHound_Off->rect = { 1220,123,18,17 };
		}

		/*{
			App->audio->PlayFx(INGAME_CLICK);
			isBuilding = true;
			type = Entity::entityType::WAR_HOUND;
			collider.dimensions = { 1,1 };
		}
		*/
		//App->audio->PlayFx(WRONG);

		break;


	case::UI_Element::Action::ACT_CAST_INVULNERABILITY:
		if (inmune == false && Invulnerable_abilities>0)
		{
			timer_ref_sec = App->scene->worldseconds;
			timer_ref_min = App->scene->worldminutes;
			desired_second = timer_ref_sec + 10;
			desired_min = timer_ref_min;

			if (desired_second >= 60)
			{
				int extra = 60 - desired_second;
				desired_second = extra;
				desired_min++;
			}
			inmune = true;
			Invulnerable_abilities--;
		}
		break;


	case::UI_Element::Action::ACT_CAST_MISSILES:
		//
		App->audio->PlayFx(WRONG);
		break;

	case::UI_Element::Action::ACT_UPGRADE:
		(*building_selected)->upgrade = true;
		break;

	case::UI_Element::Action::ACT_REPAIR:
		(*building_selected)->repair = true;
		sprintf_s(repair_cost_label, " - %i $", (*building_selected)->repair_cost); //Update label
		break;
	case::UI_Element::Action::RESUME_PAUSE:
		
		App->scene->world_seconds.Start();
		App->render->zoom = 0.77;
		Pause_UI->visible = false;
		isPaused = false;
		App->audio->PlayFx(INGAME_CLICK);
		App->scene->pause = !App->scene->pause;

		currentUI = last_currentUI;
		UpdateVisibility();
		UpdateFocus(currentUI);
		break;
	case::UI_Element::Action::SETTINGS_PAUSE:
		// Open settings menu
		Settings_UI->visible = true;
		currentUI = CURR_PAUSE_SETTINGS;
		UpdateFocus(currentUI);
		App->audio->PlayFx(INGAME_CLICK);
		break;
	case::UI_Element::Action::ABORT_PAUSE:
		currentUI = CURR_PAUSE_ABORT;
		UpdateVisibility();
		UpdateFocus(currentUI);
		App->audio->PlayFx(INGAME_CLICK);
		break;
	case::UI_Element::Action::RESTART:
		
		App->audio->PlayFx(INGAME_CLICK);

		App->entitymanager->Disable();
		App->player2->Disable();
		App->player1->Disable();
		App->gui->Disable();
		App->map->Disable();
		App->font->Disable();

		App->scenechange->ChangeMap(-1, 2);
		App->scene->Disable();	//Here fadetoblack
		App->scene->Enable();

		App->font->Enable();
		App->map->Enable();
		App->gui->Enable();
		App->player1->Enable();
		App->player2->Enable();
		App->entitymanager->Enable();

		break;
	}
	
}

bool Player::DeleteEntity(Entity* entity)
{
	UpdateWalkabilityMap(WALKABLE, entity->collider);

	entity->CleanUp();

	if (entity->type >= Entity::entityType::TOWNHALL && entity->type <= Entity::entityType::BARRACKS) //if entity = building
	{
		list<Building*>::iterator item = buildings.begin();
		while (item != buildings.end())
		{
			if ((*item) == entity)
			{
				buildings.erase(item);
				break;
			}
				
			item++;
		}
	}
	else if (entity->type > Entity::entityType::BARRACKS) //if entity = troop
	{
		// Update Gold per Kill
		if (isPlayer1 == true)
			App->player2->gold += GoldKill(entity);

		else if (isPlayer1 == false)
			App->player1->gold += GoldKill(entity);
		// ---

		list<Troop*>::iterator item = troops.begin();
		while (item != troops.end())
		{
			if ((*item) == entity) {
				troops.erase(item);
				break;
			}
			item++;
		}
		
	}
	list<Entity*>::iterator item2 = App->entitymanager->entity_list.begin();
	while (item2 != App->entitymanager->entity_list.end())
	{
		if ((*item2) == entity)
		{
			App->entitymanager->entity_list.erase(item2);
			break;
		}

		item2++;
	}

	item2 = entities.begin();
	while (item2 != entities.end())
	{
		if ((*item2) == entity)
		{
			entities.erase(item2);
			break;
		}

		item2++;
	}
	return true;
}

void Player::Update_troop_image(int type) // Changes sprite depending on the entity type
{
	switch (type)
	{
	case Entity::entityType::SOLDIER:
		troop_icon->rect = { 662, 0, 85, 81 };
		break;

	case Entity::entityType::TANKMAN:
		troop_icon->rect = { 576, 0, 85, 81 };
		break;

	case Entity::entityType::INFILTRATOR:
		troop_icon->rect = { 832, 0, 85, 81 };
		break;

	case Entity::entityType::ENGINEER:
		troop_icon->rect = { 492, 0, 85, 81 };
		break;

	case Entity::entityType::WAR_HOUND:
		troop_icon->rect = { 746, 0, 85, 81 };
		break;

	case ABILITIES::INVULNERABLE:
		troop_icon->rect = { 576, 161, 85, 81 };
		break;

	case ABILITIES::ROCKET:
		troop_icon->rect = { 662, 161, 85, 81 };
		break;

	case ABILITIES::TANK:
		troop_icon->rect = { 492, 161, 85, 81 };
		break;

	}
}

void Player::CreateTroop(int type, int number)
{
	Entity::entityType entity_type;
	switch (type)
	{
	case 8:
		entity_type = Entity::entityType::SOLDIER;
		break;
	case 9:
		entity_type = Entity::entityType::TANKMAN;
		break;
	case 10:
		entity_type = Entity::entityType::INFILTRATOR;
		break;
	case 11:
		entity_type = Entity::entityType::ENGINEER;
		break;
	case 12:
		entity_type = Entity::entityType::WAR_HOUND;
		break;
	}

	for (int i = 0; i < number; i++)
	{
		(*building_selected)->TroopsCreated.push_back(entity_type);
	}
	
}

void Player::CreateAbility(int type, int number)
{
	switch (type)
	{
	case ABILITIES::INVULNERABLE:
		Invulnerable_abilities += number;
		break;

	case ABILITIES::ROCKET:
		Rocket_abilities += number;
		break;

	case ABILITIES::TANK:
		Tank_abilities += number;
		break;
	}
}

void Player::UpdateGeneralUI(Entity* building)
{
	sprintf_s(name_label, "%s", building->GetName(building->type));

	sprintf_s(health_label, "HP: %i", building->health);

	sprintf_s(level_label, "Lvl: %i", building->level + 1);

	sprintf_s(repair_cost_label, " - %i $", building->repair_cost);

	if (building->type == Entity::entityType::BARRACKS)
	{
		Create_troops->visible = true;
	}
	else
	{
		Create_troops->visible = false;
	}

	if (building->type == Entity::entityType::COMMAND_CENTER)
	{
		Create_abilities->visible = true;
	}
	else
	{
		Create_abilities->visible = false;
	}

}

void Player::DrawBuildingCollider(int type, bool isPlayer1)
{
	
	if(type == Entity::entityType::TOWNHALL && isPlayer1 == true)
	{

		selected_texture.x = (*building_selected)->position.first - ((*building_selected)->collider.dimensions.first * 20);

		selected_texture.y = (*building_selected)->position.second - (*building_selected)->Current_Animation->frames->h + ((*building_selected)->collider.dimensions.second * 20);
		selected_texture.w = (*building_selected)->Current_Animation->frames->w;
		selected_texture.h = (*building_selected)->Current_Animation->frames->h;
	}
	else if (type == Entity::entityType::TOWNHALL && isPlayer1 == false)
	{
		selected_texture.x = (*building_selected)->position.first - ((*building_selected)->collider.dimensions.first * 8);
		selected_texture.y = (*building_selected)->position.second - (*building_selected)->Current_Animation->frames->h + ((*building_selected)->collider.dimensions.second * 40);
		selected_texture.w = (*building_selected)->Current_Animation->frames->w;
		selected_texture.h = (*building_selected)->Current_Animation->frames->h;
	}
	else if (type == Entity::entityType::MINES || type == Entity::entityType::BARRACKS)
	{
		selected_texture.x = (*building_selected)->position.first - ((*building_selected)->collider.dimensions.first * 20);
		selected_texture.y = (*building_selected)->position.second - (*building_selected)->Current_Animation->frames->h + ((*building_selected)->collider.dimensions.second * 20);
		selected_texture.w = (*building_selected)->Current_Animation->frames->w;
		selected_texture.h = (*building_selected)->Current_Animation->frames->h;
	}
	else
	{
		selected_texture.x = (*building_selected)->position.first;
		selected_texture.y = (*building_selected)->position.second - (*building_selected)->Current_Animation->frames->h + ((*building_selected)->collider.dimensions.second * 20);
		selected_texture.w = (*building_selected)->Current_Animation->frames->w;
		selected_texture.h = (*building_selected)->Current_Animation->frames->h;
	}




	if (isPlayer1)
		App->render->DrawQuad(selected_texture, 255, 0, 0, 100, true);
	else
		App->render->DrawQuad(selected_texture, 0, 0, 255, 100, true);
}


void Player::Blit_Info()
{
	SDL_Rect section;
	SDL_Rect sec2 = { -1090, 668, 351, 20 };

	if ((*focus) == Def_AOE_icon)
	{
		section = { 0, 1234, 351, 87 };
	}
	else if ((*focus) == Def_Target_icon)
	{
		section = { 359, 1234, 351, 87 };
	}
	else if ((*focus) == Mines_icon)
	{
		section = { 716, 1234, 351, 87 };
	}
	else if ((*focus) == Barracks_icon)
	{
		section = { 1074, 1234, 351, 87 };
	}
	else if (currentUI == CURRENT_UI::CURR_DEPLOY)
	{
		section = { 0, 1520, 351, 87 };
	}
	else if (currentUI==CURRENT_UI::CURR_CREATE_TROOPS && UI_troop_type == Entity::entityType::SOLDIER)
	{
		section = { 0, 1331, 351, 87 };
	}
	else if (currentUI == CURRENT_UI::CURR_CREATE_TROOPS && UI_troop_type == Entity::entityType::TANKMAN)
	{
		section = { 359, 1331, 351, 87 };
	}
	else if (currentUI == CURRENT_UI::CURR_CREATE_TROOPS && UI_troop_type == Entity::entityType::ENGINEER)
	{
		section = { 716, 1331, 351, 87 };
	}
	else if (currentUI == CURRENT_UI::CURR_CREATE_TROOPS && UI_troop_type == Entity::entityType::INFILTRATOR)
	{
		section = { 1074, 1331, 351, 87 };
	}
	else if (currentUI == CURRENT_UI::CURR_CREATE_TROOPS && UI_troop_type == Entity::entityType::WAR_HOUND)
	{
		section = { 0, 1426, 351, 87 };
	}
	else if ((*focus) == Build_icon)
	{
		section = { 1425, 1234, 351, 87 };
	}
	else if ((*focus) == Deploy_icon)
	{
		section = { 1425, 1331, 351, 87 };
	}
	else if ((*focus) == Cast_icon)
	{
		section = { 1425, 1426, 351, 87 };
	}
	else if ((*focus) == Missiles_icon || (currentUI == CURRENT_UI::CURR_CREATE_ABILITIES && UI_troop_type == ABILITIES::ROCKET))
	{
		section = { 1075, 1426, 351, 87 };
	}
	else if ((*focus) == Cast2_icon || (currentUI == CURRENT_UI::CURR_CREATE_ABILITIES && UI_troop_type == ABILITIES::TANK))
	{
		section = { 360, 1426, 351, 87 };
	}
	else if ((*focus) == Cast3_icon || (currentUI == CURRENT_UI::CURR_CREATE_ABILITIES && UI_troop_type == ABILITIES::INVULNERABLE))
	{
		section = { 716, 1426, 351, 87 };
	}
	else if (currentUI == CURRENT_UI::CURR_GENERAL)
	{
		if ((*building_selected)->type == Entity::entityType::DEFENSE_AOE)
		{
			section = { 0, 1234, 351, 87 };
		}
		else if ((*building_selected)->type == Entity::entityType::BARRACKS)
		{
			section = { 1074, 1234, 351, 87 };
		}
		else if ((*building_selected)->type == Entity::entityType::MAIN_DEFENSE)
		{
			section = { 359, 1234, 351, 87 };
		}
		else if ((*building_selected)->type == Entity::entityType::MINES)
		{
			section = { 716, 1234, 351, 87 };
		}
		else if ((*building_selected)->type == Entity::entityType::TOWNHALL)
		{
			section = { 1796, 1234, 351, 87 };
		}
		else if ((*building_selected)->type == Entity::entityType::COMMAND_CENTER)
		{
			section = { 1796, 1331, 351, 87 };
		}
		else if ((*building_selected)->type == Entity::entityType::DEFENSE_TARGET)
		{
			section = { 1796, 1426, 351, 87 };
		}
		
	}

	if (isPlayer1)
		App->render->Blit(App->gui->GetAtlas(), 470, 1590, &section);
	else
		App->render->Blit(App->gui->GetAtlas(), -1090, 690, &section);

	App->render->DrawQuad(sec2, 0, 0, 0, 150);
}

void Player::ChangeTroopsState()
{
	
	if (Soldier_Offensive && Tankman_Offensive && Infiltrator_Offensive && Engineer_Offensive && WarHound_Offensive)
	{
		Soldier_Offensive = false;
		Tankman_Offensive = false;
		Infiltrator_Offensive = false;
		Engineer_Offensive = false;
		WarHound_Offensive = false;
	}
	else
	{
		Soldier_Offensive = true;
		Tankman_Offensive = true;
		Infiltrator_Offensive = true;
		Engineer_Offensive = true;
		WarHound_Offensive = true;
	}

	if (Soldier_Offensive)
	{
		Soldier_Deff->rect = { 1219,98,20,21 };
		Soldier_Off->rect = { 1195,123,18,17 };
	}
	else
	{
		Soldier_Deff->rect = { 1195,98,20,21 };
		Soldier_Off->rect = { 1220,123,18,17 };
	}

	if (Tankman_Offensive)
	{
		Tankman_Deff->rect = { 1219,98,20,21 };
		Tankman_Off->rect = { 1195,123,18,17 };
	}
	else
	{
		Tankman_Deff->rect = { 1195,98,20,21 };
		Tankman_Off->rect = { 1220,123,18,17 };
	}

	if (Infiltrator_Offensive)
	{
		Infiltrator_Deff->rect = { 1219,98,20,21 };
		Infiltrator_Off->rect = { 1195,123,18,17 };
	}
	else
	{
		Infiltrator_Deff->rect = { 1195,98,20,21 };
		Infiltrator_Off->rect = { 1220,123,18,17 };
	}

	if (Engineer_Offensive)
	{
		Engineer_Deff->rect = { 1219,98,20,21 };
		Engineer_Off->rect = { 1195,123,18,17 };
	}
	else
	{
		Engineer_Deff->rect = { 1195,98,20,21 };
		Engineer_Off->rect = { 1220,123,18,17 };
	}

	if (WarHound_Offensive)
	{
		WarHound_Deff->rect = { 1219,98,20,21 };
		WarHound_Off->rect = { 1195,123,18,17 };
	}
	else
	{
		WarHound_Deff->rect = { 1195,98,20,21 };
		WarHound_Off->rect = { 1220,123,18,17 };
	}
}


