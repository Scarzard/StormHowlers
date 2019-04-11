#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Render.h"
#include "Window.h"
#include "Map.h"
#include "Pathfinding.h"
#include "Scene.h"
#include "MainMenu.h"
#include "Transitions.h"
#include "EntityManager.h"
#include "Gui.h"
#include "Fonts.h"
#include "UI_Element.h"
#include "Player.h"
#include "Audio.h"

#include "Brofiler\Brofiler.h"

Scene::Scene() : Module()
{
	name.assign("scene");
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake(pugi::xml_node& config)
{
	bool ret = true;
	LOG("Loading Scene");

	for (pugi::xml_node map = config.child("map_name"); map; map = map.next_sibling("map_name"))
	{
		string* data = new string;
		data->assign(map.attribute("name").as_string());
		map_names.push_back(data);
	}
	fade_time = config.child("fade_time").attribute("value").as_float();

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	bool ret = false;

	App->map->Load(map_names.front()->data());
	//current_track = App->audio->tracks_path.front();
	//App->audio->PlayMusic(PATH(App->audio->folder_music.data(), current_track.data()));

	// Variables init
	currentMap = 0;
	pause = false;
	godmode = false;
	to_end = false;
	change = false;
	App->player1->isPlayer1 = true;
	App->player2->isPlayer1 = false;
	App->map->debug_tex = App->tex->Load("maps/meta.png");

	//walkability map
	Wmap = NULL;
	int w, h;
	if (App->map->CreateWalkabilityMap(w, h, &Wmap))
	{
		App->pathfinding->SetMap(w, h, Wmap);
		LOG("Create walkability map");
	}
	debug_tex = App->tex->Load("maps/pathfinding.png");

	//animation testing
	if (spritesheet123 == nullptr)
		spritesheet123 = App->tex->Load("textures/Buildings.png");

	//--------- CREATE MAIN BUILDINGS -------------// (falta cambiar posicion)

	//--- PLAYER 1
	App->entitymanager->AddEntity(true, Entity::entityType::TOWNHALL, { 50,50 });
	App->entitymanager->AddEntity(true, Entity::entityType::MAIN_DEFENSE, { 50,50 });
	App->entitymanager->AddEntity(true, Entity::entityType::COMMAND_CENTER, { 50,50 });
	App->entitymanager->AddEntity(true, Entity::entityType::WALLS, { 50,50 });

	//--- PLAYER 1
	App->entitymanager->AddEntity(false, Entity::entityType::TOWNHALL, { 50,50 });
	App->entitymanager->AddEntity(false, Entity::entityType::MAIN_DEFENSE, { 50,50 });
	App->entitymanager->AddEntity(false, Entity::entityType::COMMAND_CENTER, { 50,50 });
	App->entitymanager->AddEntity(false, Entity::entityType::WALLS, { 50,50 });

	string track = App->audio->folder_music + "/Test.ogg"; 
	App->audio->PlayMusic(track.c_str()); 

	//--------- CREATE GUI -----------//  (Falta poner position y size)
	ui_timer = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { App->win->width/2 ,00 }, { 0,0 }, nullptr, true, { false, false }, "Timer: 0s");
	ui_timer->color = { 250,250,250,250 };

	//--- PLAYER 1
	//App->player1->Health_UI = App->gui->AddUIElement(true, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { x,y }, { w,h }, nullptr, true);
	//App->player1->Gold_UI = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { x,y }, { w,h }, nullptr, true, { false,false }, "$");
	App->player1->Main_UI = App->gui->AddUIElement(true, UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { 0,0 }, { 566, 163 }, nullptr, true);
	App->player1->Main_UI->rect = { 0,245,566,163 };
	App->player1->Build_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_GOTO_BUILD, { 275,55 }, { 80, 81 }, App->player1->Main_UI, true);
	App->player1->Deploy_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_GOTO_DEPLOY, { 378,55 }, { 80, 81 }, App->player1->Main_UI, true);
	App->player1->Cast_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_GOTO_CAST, { 482,55 }, { 80,81 }, App->player1->Main_UI, true);


	App->player1->Build_UI = App->gui->AddUIElement(true, UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { 0,0 }, { 566, 163 }, nullptr, false);
	App->player1->Build_UI->rect = { 569, 246, 566, 163 };
	App->player1->Def_AOE_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_AOE, { 119, 55 }, { 85, 81 } , App->player1->Build_UI, false);
	App->player1->Def_Target_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_TARGET, { 222, 55 }, { 85, 81 }, App->player1->Build_UI, false);
	//App->player1->Mines_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_MINE, { x,y }, { 39,40 }, App->player1->Build_UI, false);
	//App->player1->Barracks_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_BARRACKS, { x,y }, { 39,40 }, App->player1->Build_UI, false);

	App->player1->Deploy_UI = App->gui->AddUIElement(true, UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { 0,0 }, { 566, 163 }, nullptr, false);
	App->player1->Deploy_UI->rect = { 569, 411, 566, 163 };
	App->player1->Soldier_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_SOLDIER, { 68, 55 }, { 85, 81 }, App->player1->Deploy_UI, false);
	App->player1->Tankman_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_TANKMAN, { 171, 55 }, { 85, 81 }, App->player1->Deploy_UI, false);

	//------ These coords are for the 3rd slot in the menu { 275, 55 }, { 85, 81 }
	//App->player1->Infiltrator_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_INFILTRATOR, { 275, 55 }, { 85, 81 }, App->player1->Deploy_UI, false);
	//------ These coords are for the 4th slot in the menu { 380, 55 }, { 85, 81 }
	//App->player1->Engineer_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_ENGINEER, { 380, 55 }, { 85, 81 }, App->player1->Deploy_UI, false);
	//App->player1->War_hound_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_WARHOUND, { x,y }, { 85, 81 }, App->player1->Deploy_UI, false);

	App->player1->Cast_UI = App->gui->AddUIElement(true, UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { 0,0 }, { 166,79 }, nullptr, false);
	App->player1->Cast_UI->rect = { 569, 575, 566, 163 };
	//App->player1->Missiles_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_CAST_MISSILES, { x,y }, { 39,40 }, player.Cast_UI, false);
	//App->player1->Cast2_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_CAST_2, { x,y }, { 39,40 }, App->player1->Cast_UI, false);
	//App->player1->Cast3_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_CAST_3, { x,y }, { 39,40 }, App->player1->Cast_UI, false);

	//App->player1->General_UI = App->gui->AddUIElement(true, UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { 0,0 }, { w,h }, nullptr, false);
	//App->player1->Upgrade_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_UPGRADE, { x,y }, { w,h }, App->player1->General_UI, false);
	//App->player1->Repair_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_REPAIR, { x,y }, { w,h }, App->player1->General_UI, false);
	////falta poner los pointer a los datos del edificio seleccionado (ahora esta como "data") 
	//App->player1->Name_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { x,y }, { w,h }, App->player1->General_UI, false, { false, false }, "data");
	//App->player1->Level_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { x,y }, { w,h }, App->player1->General_UI, false, { false, false }, "data");
	//App->player1->Health_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { x,y }, { w,h }, App->player1->General_UI, false, { false, false }, "data");
	//App->player1->Damage_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { x,y }, { w,h }, App->player1->General_UI, false, { false, false }, "data");
	//App->player1->Prod_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { x,y }, { w,h }, App->player1->General_UI, false, { false, false }, "data");
	//App->player1->Capacity_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { x,y }, { w,h }, App->player1->General_UI, false, { false, false }, "data");


	//--- PLAYER 2
	//App->player2->Health_UI = App->gui->AddUIElement(false, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { x,y }, { w,h }, nullptr, true);
	//App->player2->Gold_UI = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { x,y }, { w,h }, nullptr, true, { false,false }, "$");
	App->player2->Main_UI = App->gui->AddUIElement(false, UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { App->win->width ,App->win->height + 122 }, { 566,163 }, nullptr, true);
	App->player2->Main_UI->rect = { 0, 408, 566, 163 };
	App->player2->Build_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_GOTO_BUILD, { 275,55 }, { 80, 81 }, App->player2->Main_UI, true);
	App->player2->Deploy_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_GOTO_DEPLOY, { 378,55 }, { 80, 81 }, App->player2->Main_UI, true);
	App->player2->Cast_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_GOTO_CAST, { 482,55 }, { 80,81 }, App->player2->Main_UI, true);


	App->player2->Build_UI = App->gui->AddUIElement(false, UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { App->win->width - 166,App->win->height - 79 }, { 166,79 }, nullptr, false);
	App->player2->Build_UI->rect = { 166,0,166,79 };
	App->player2->Def_AOE_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_AOE, { 2,37 }, { 39,40 } , App->player2->Build_UI, true);
	//App->player2->Def_Target_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_TARGET, { x,y }, { 39,40 }, App->player2->Build_UI, false);
	//App->player2->Mines_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_MINE, { x,y }, { 39,40 }, player.P2, false);
	//App->player2->Barracks_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_BARRACKS, { x,y }, { 39,40 }, App->player2->Build_UI, false);

	App->player2->Deploy_UI = App->gui->AddUIElement(false, UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { App->win->width - 166,App->win->height - 79 }, { 166,79 }, nullptr, false);
	App->player2->Deploy_UI->rect = { 166,0,166,79 };
	//App->player2->Soldier_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_SOLDIER, { x,y }, { 39,40 }, App->player2->Deploy_UI, false);
	//App->player2->Tankman_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_TANKMAN, { x,y }, { 39,40 }, App->player2->Deploy_UI, false);
	//App->player2->Infiltrator_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_INFILTRATOR, { x,y }, { 39,40 }, App->player2->Deploy_UI, false);
	//App->player2->Engineer_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_ENGINEER, { x,y }, { 39,40 }, App->player2->Deploy_UI, false);
	//App->player2->War_hound_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_WARHOUND, { x,y }, { 39,40 }, App->player2->Deploy_UI, false);

	App->player2->Cast_UI = App->gui->AddUIElement(false, UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { App->win->width - 166,App->win->height - 79 }, { 166,79 }, nullptr, false);
	App->player2->Cast_UI->rect = { 166,0,166,79 };
	//App->player2->Missiles_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_CAST_MISSILES, { x,y }, { 39,40 }, App->player2->Cast_UI, false);
	//App->player2->Cast2_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_CAST_2, { x,y }, { 39,40 }, App->player2->Cast_UI, false);
	//App->player2->Cast3_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_CAST_3, { x,y }, { 39,40 }, App->player2->Cast_UI, false);

	//App->player2->General_UI = App->gui->AddUIElement(false, UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { 0,0 }, { w,h }, nullptr, false);
	//App->player2->Upgrade_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_UPGRADE, { x,y }, { w,h }, App->player2->General_UI, false);
	//App->player2->Repair_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_REPAIR, { x,y }, { w,h }, App->player2->General_UI, false);
	////falta poner los pointer a los datos del edificio seleccionado (ahora esta como "data") 
	//App->player2->Name_text = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { x,y }, { w,h }, App->player2->General_UI, false, { false, false }, "data");
	//App->player2->Level_text = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { x,y }, { w,h }, App->player2->General_UI, false, { false, false }, "data");
	//App->player2->Health_text = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { x,y }, { w,h }, App->player2->General_UI, false, { false, false }, "data");
	//App->player2->Damage_text = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { x,y }, { w,h }, player.General_UI, false, { false, false }, "data");
	//App->player2->Prod_text = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { x,y }, { w,h }, App->player2->General_UI, false, { false, false }, "data");
	//App->player2->Capacity_text = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { x,y }, { w,h }, App->player2->General_UI, false, { false, false }, "data");

	// --- CURSORS
	//Cursor player 1 ------------
	if (App->player1->gamepad.Connected == true)
	{
		App->player1->cursor.position.first = 300;
		App->player1->cursor.position.second = 300;
		App->player1->cursor.area.x = App->player1->cursor.area.y = 0;
		App->player1->cursor.area.w = App->player1->cursor.area.h = 25;
	}
	else
		LOG("...Player1 GamePad Not Connected");

	//Cursor player 2 ------------
	if (App->player2->gamepad.Connected == true)
	{
		App->player2->cursor.position.first = 500;
		App->player2->cursor.position.second = 300;
		App->player2->cursor.area.x = 27;
		App->player2->cursor.area.y = 0;
		App->player2->cursor.area.w = App->player2->cursor.area.h = 25;
	}
	else
		LOG("...Player2 GamePad Not Connected");

	if (cursor_tex == nullptr)
		cursor_tex = App->tex->Load("textures/Cursors.png");

	//-----------
	SpawnEntities();

	App->font->font_iterator = App->font->fonts.begin();
	// timer start
	world_clock.Start();
	world_seconds.Start();
	size_timer.Start();


	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	BROFILER_CATEGORY("Scene PreUpdate", Profiler::Color::DarkOrange);
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	BROFILER_CATEGORY("Scene Update", Profiler::Color::DarkOrange);

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN || App->player1->gamepad.Controller[BUTTON_B] == KEY_DOWN) //return to main_ui player1 
	{
		App->player1->currentUI = Player::CURRENT_UI::CURR_MAIN;
		App->player1->UpdateVisibility();
		App->player1->isBuilding = false;
	}
	else if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN || App->player2->gamepad.Controller[BUTTON_B] == KEY_DOWN) //return to main_ui player2
	{
		App->player2->currentUI = Player::CURRENT_UI::CURR_MAIN;
		App->player2->UpdateVisibility();
		App->player2->isBuilding = false;
	}
	//else if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) //Start from second level
	//{
	//	Load_level(1);
	//	currentMap = 1;
	//}
	//else if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN) //Go to main menu
	//{
	//	App->scenechange->SwitchScene(App->main_menu, App->scene);
	//}
	//else if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) //Save game state
	//{
	//	App->SaveGame();
	//}
	//else if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) //Load game state
	//{
	//	App->LoadGame();
	//}
	//else if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN) //Debug UI
	//{
	//	App->gui->UI_Debug = !App->gui->UI_Debug;
	//}
	else if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) //View colliders
	{
		App->map->debug = !App->map->debug;
		App->entitymanager->draw_path = !App->entitymanager->draw_path;
	}
	else if (App->input->GetKey(SDL_SCANCODE_L) == KEY_REPEAT) //View colliders
	{
		App->render->camera.x+=5;
	}
	else if (App->input->GetKey(SDL_SCANCODE_I) == KEY_REPEAT) //View colliders
	{
		App->render->zoom += 0.01f;
	}
	else if (App->input->GetKey(SDL_SCANCODE_K) == KEY_REPEAT) //View colliders
	{
		App->render->zoom -= 0.01f;
	}

	

	if (true) {

		if (App->input->GetMouseButtonDown(SDL_BUTTON_MIDDLE) == KEY_DOWN) {
			LOG("CLICK");
			App->input->GetMousePosition(camera_motion.first, camera_motion.second);
			start_motion = true;
		}

		if (App->input->GetMouseButtonDown(SDL_BUTTON_MIDDLE) == KEY_UP) {
			LOG("CLICK UP");
			start_motion = false;
			//App->input->GetMousePosition(camera_motion.x, camera_motion.y);
		}
		if (start_motion) {

			pair<int,int> final;
			App->input->GetMousePosition(final.first, final.second);
			SDL_SetRenderDrawColor(App->render->renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
			if (App->map->debug)SDL_RenderDrawLine(App->render->renderer, camera_motion.first, camera_motion.second, final.first, final.second);

			App->input->GetMouseMotion(final.first, final.second);
			LOG("final_motio: %d,%d", final.first, final.second);
			LOG("last_motion: %d,%d", last_motion.first, last_motion.second);

			last_motion.first -= final.first;
			last_motion.second -= final.second;
			LOG("minus_motio: %d,%d", last_motion.first, last_motion.second);

			if (last_motion.first != 0 && last_motion.second != 0) {
				App->render->camera.x += final.first;
				App->render->camera.y += final.second;
			}

			last_motion = { final.first, final.second };
		}
	}

	//else if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) //Godmode
	//{
	//	godmode = !godmode;
	//}
	//else if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN) //Framerate Cap On/Off
	//{
	//	App->fpsCapON = !App->fpsCapON;
	//}

	// testing timer
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
	{
		if (pausetimer == false)
		{
			//if the pause timer is false, the clock is running and you want to stop it
			pausetimer=true;
			world_seconds.Stop();
		}
		else if (pausetimer == true)
		{
			//if the pause timer is true, the clock is stop and you want to start it
			pausetimer = false;
			world_seconds.Start();
			
		}
	}

	reference_active = change_font_size;
	

	if (((worldseconds >= 0 && worldseconds <= 1) && worldminutes==0) || ((worldseconds >= 0 && worldseconds <= 1) && worldminutes == 5 ) || (worldseconds >=0 && worldseconds <= 1 && worldminutes == 7) || (worldseconds >= 0 && worldseconds <= 1 && worldminutes == 9) && pausetimer==false)
	{
		change_font_size = true;
		ui_timer->visible = true;
		
	
	}
	
	
	
	if (ui_timer->visible == true && change_font_size == true && pausetimer ==false)
	{
		
		if (reference_active != change_font_size)
		{
			
			increase_size = true;
			App->font->font_iterator = App->font->fonts.begin();
			App->font->size = 17;
			if ((worldseconds <= 1) && worldminutes==0 )
			{
				App->audio->PlayFx(MIN10);
			}
			else if ((worldseconds <= 1) && worldminutes == 5)
			{
				App->audio->PlayFx(MIN5);
			}
			else if ((worldseconds <= 1) && worldminutes == 7)
			{
				App->audio->PlayFx(MIN3);
			} 
			else if ((worldseconds <= 1)&& worldminutes == 9)
			{
				App->audio->PlayFx(MIN1);
			}
		}

		if (worldseconds <=15)
		{
			if (increase_size == true && App->font->size < 61)
			{
				App->font->size++;
				App->font->actual_font = *App->font->font_iterator;
				App->font->font_iterator++;

			}
			else if (increase_size == false && App->font->size > 15)
			{
				App->font->size--;
				App->font->actual_font = *App->font->font_iterator;
				App->font->font_iterator--;

			}


			if (worldseconds >= 10 && increase_size==true)
			{
				
				increase_size = !increase_size;
				increase_decresease++;
				if (increase_size == true)
				{
					App->font->size = 0;
				}
				else if (increase_size == false)
				{
					App->font->size = 60;
				}
			}
			else if (worldseconds >= 15 && increase_size == false)
			{
				increase_decresease++;
			}
			

			 if (increase_decresease>=2 )
			{
				change_font_size = false;
				reference_active = false;
				increase_size = true;
				increase_decresease = 0;
				ui_timer->visible = false;
			
				
			}
		}

	}




	//----
	App->map->Draw(dt);
	//App->entitymanager->Draw(dt);
	App->gui->Draw();

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	BROFILER_CATEGORY("Scene PostUpdate", Profiler::Color::DarkOrange);

	bool ret = true;

	//--- Pause
	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		pause = !pause;
	}

	//--- Update Player 1 GUI
	list<UI_Element*>::reverse_iterator item = App->player1->UI_elements.rbegin();
	while (item != App->player1->UI_elements.rend())
	{
		// timer 
		if ((*item) == ui_timer && pausetimer == false) 
		{


			if (world_seconds.ReadSec() >= 1)
			{
				world_seconds.Start();
				worldseconds++;

				if (worldseconds >= 60)
				{
					worldseconds = 0;
					worldminutes++;
				}

				if (size_timer.runningRead() == true && size_timer.ReadSec() >=1)
				{
					size_timer_count++;
					size_timer.Start();
				}

			}

			sprintf_s(current_time, "TIME: %u:%u",worldminutes, worldseconds);
			(*item)->label = current_time;
			break;
		}
		//
		
		else if ((*item)->visible == true)
		{
			
			//if (((App->gui->CheckMousePos(*item) == true && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) != KEY_REPEAT) ||
			//	(App->player1->CheckCursorPos(*item) == true && App->player1->gamepad.Controller[BUTTON_A] != KEY_REPEAT)) && (*item)->dragging == false) //hovering
			//{
			//	(*item)->state = UI_Element::State::HOVER;
			//}
			if (((App->gui->CheckClick(*item) == true && App->gui->CheckMousePos(*item) == true) ||
				(App->player1->CheckCursorClick(*item) == true && App->player1->CheckCursorPos(*item) == true)) && (*item)->state == UI_Element::State::HOVER) //on-click
			{
				if ((*item)->dragable.x == false && (*item)->dragable.y == false) //if not dragable
				{
					(*item)->state = UI_Element::State::LOGIC; //do logic
					if ((*item)->locked == true) //if locked
					{
						//App->audio->PlayFx(LOCKED);
					}
					else
					{
						App->player1->DoLogic(*item);
					}
				}
				else //drag
				{
					(*item)->dragging = true;
					(*item)->Drag();

					////--- Do logic
					//if ((*item)->action == UI_Element::Action::ADJUST_VOL)
					//{
					//}

					////--- Check limits
					//if ((*item)->globalpos.first <= limit) //left limit
					//	(*item)->globalpos.first = limit;
					//else if ((*item)->globalpos.first >= limit) //right limit
					//	(*item)->globalpos.first = limit;
				}
			}
			//else if (App->gui->CheckMousePos(*item) == false && App->player1->CheckCursorPos(*item) == false && (*item)->state != UI_Element::State::DRAG) //change to idle
			//{
			//	(*item)->state = UI_Element::State::IDLE;
			//}
		}
		App->gui->UpdateState(*item);
		item++;
	}

	//--- Update Player 2 GUI
	item = App->player2->UI_elements.rbegin();
	while (item != App->player2->UI_elements.rend())
	{
		if ((*item)->visible == true)
		{

			if (((App->gui->CheckMousePos(*item) == true && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) != KEY_REPEAT) ||
				(App->player2->CheckCursorPos(*item) == true && App->player2->gamepad.Controller[BUTTON_A] != KEY_REPEAT)) && (*item)->dragging == false) //hovering
			{
				(*item)->state = UI_Element::State::HOVER;
			}
			if (((App->gui->CheckClick(*item) == true && App->gui->CheckMousePos(*item) == true) ||
				(App->player2->CheckCursorClick(*item) == true && App->player2->CheckCursorPos(*item) == true)) && (*item)->state == UI_Element::State::HOVER) //on-click
			{
				if ((*item)->dragable.x == false && (*item)->dragable.y == false) //if not dragable
				{
					(*item)->state = UI_Element::State::LOGIC; //do logic
					if ((*item)->locked == true) //if locked
					{
						//App->audio->PlayFx(LOCKED);
					}
					else
					{
						App->player2->DoLogic(*item);
					}
				}
				else //drag
				{
					(*item)->dragging = true;
					(*item)->Drag();

					////--- Do logic
					//if ((*item)->action == UI_Element::Action::ADJUST_VOL)
					//{
					//}

					////--- Check limits
					//if ((*item)->globalpos.first <= limit) //left limit
					//	(*item)->globalpos.first = limit;
					//else if ((*item)->globalpos.first >= limit) //right limit
					//	(*item)->globalpos.first = limit;
				}
			}
			else if (App->gui->CheckMousePos(*item) == false && App->player2->CheckCursorPos(*item) == false && (*item)->state != UI_Element::State::DRAG) //change to idle
			{
				(*item)->state = UI_Element::State::IDLE;
			}
		}
		App->gui->UpdateState(*item);
		item++;
	}
	App->gui->UpdateChildren();

	//--- Draw Cursors
	if (App->player1->gamepad.Connected == true)
		App->render->Blit(cursor_tex, App->player1->cursor.position.first, App->player1->cursor.position.second, &App->player1->cursor.area);

	if (App->player2->gamepad.Connected == true)
		App->render->Blit(cursor_tex, App->player2->cursor.position.first, App->player2->cursor.position.second, &App->player2->cursor.area);

	//--- Change map with fade
	if (to_end == true && App->scenechange->IsChanging() == false)
	{
		if (currentMap < map_names.size() - 1)
		{
			ret = App->scenechange->ChangeMap(++currentMap, fade_time);
		}
		else
		{
			currentMap = 0;
			ret = App->scenechange->ChangeMap(currentMap, fade_time);
		}
		to_end = false;
	}

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	App->tex->UnLoad(debug_tex);
	App->tex->UnLoad(cursor_tex);

	debug_tex = nullptr;
	cursor_tex = nullptr;

	RELEASE_ARRAY(Wmap);

	return true;
}


bool Scene::Load(pugi::xml_node& data)
{
	LOG("IsChanging: %i", App->scenechange->IsChanging());

	if (currentMap != data.child("currentMap").attribute("num").as_int())
	{
		LOG("Calling switch maps");
		currentMap = data.child("currentMap").attribute("num").as_int();
		App->map->SwitchMaps(map_names[data.child("currentMap").attribute("num").as_int()]);
	}
	return true;
}

bool Scene::Save(pugi::xml_node& data) const
{
	data.append_child("currentMap").append_attribute("num") = currentMap;
	return true;
}

bool Scene::Load_level(int map)
{
	//App->entitymanager->DeleteEntities();
	App->map->SwitchMaps(map_names[map]);
	SpawnEntities();

	return true;
}

void Scene::SpawnEntities() //
{
	//App->entitymanager->DeleteEntities();
	SpawnEnemies();

	pugi::xml_document	config_file;
	pugi::xml_node		config;

	config = App->LoadConfig(config_file);

	//Entity* player = App->entitymanager->AddEntity(Entity::entityType::PLAYER, { 0,0 }, { 0,0 });
	//player->Awake(config.child(App->entitymanager->name.data()));
	//player->Start();
}

//void Scene::changeSize(float time, int maxsize)
//{
//	Timer time
//}


void Scene::SpawnEnemies() //
{
	//for (p2List_item<ObjectsGroup*>* object = App->map->data.objLayers.start; object; object = object->next)
	//{
	//	if (object->data->name == ("Enemies"))
	//	{
	//		for (p2List_item<ObjectsData*>* objectdata = object->data->objects.start; objectdata; objectdata = objectdata->next)
	//		{
	//		}
	//	}
	//	if (object->data->name == ("Collision"))
	//	{
	//		for (p2List_item<ObjectsData*>* objectdata = object->data->objects.start; objectdata; objectdata = objectdata->next)
	//		{
	//		}
	//	}
	//}
}