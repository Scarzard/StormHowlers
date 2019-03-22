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
	P1.currentUI = CURR_MAIN;
	P1.isPlayer1 = true;
	P2.currentUI = CURR_MAIN;
	P2.isPlayer1 = false;
	pause = false;
	godmode = false;
	to_end = false;
	change = false;

	//walkability map
	int w, h;
	uchar* data = NULL;
	if (App->map->CreateWalkabilityMap(w, h, &data))
	{
		App->pathfinding->SetMap(w, h, data);

		LOG("Create walkability");
	}
	RELEASE_ARRAY(data);
	debug_tex = App->tex->Load("maps/pathfinding.png");


	//--------- CREATE GUI ----------  //(Falta poner position y size)

	//--- PLAYER 1
	//P1.Health_UI = App->gui->AddUIElement(UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { x,y }, { w,h }, nullptr, true);
	//P1.Gold_UI = App->gui->AddUIElement(UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { x,y }, { w,h }, nullptr, true, { false,false }, "$");
	P1.Main_UI = App->gui->AddUIElement(UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { 0,0 }, { 166,79 }, nullptr, true);
	P1.Main_UI->rect = { 0,79,166,79 };
	P1.Build_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_GOTO_BUILD, { 13,2 }, { 39,40 }, P1.Main_UI, true);
	P1.Deploy_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_GOTO_DEPLOY, { 64,2 }, { 39,40 }, P1.Main_UI, true);
	P1.Cast_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_GOTO_CAST, { 115,2 }, { 39,40 }, P1.Main_UI, true);

	P1.Build_UI = App->gui->AddUIElement(UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { 0,0 }, { 166,79 }, nullptr, false);
	P1.Build_UI->rect = { 166,79,166,79 };
	//P1.Def_AOE_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_AOE, { x,y }, { 39,40 } , P1.Build_UI, false);
	//P1.Def_Target_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_TARGET, { x,y }, { 39,40 }, P1.Build_UI, false);
	//P1.Mines_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_MINE, { x,y }, { 39,40 }, P1.Build_UI, false);
	//P1.Barracks_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_BARRACKS, { x,y }, { 39,40 }, P1.Build_UI, false);

	P1.Deploy_UI = App->gui->AddUIElement(UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { 0,0 }, { 166,79 }, nullptr, false);
	P1.Deploy_UI->rect = { 166,79,166,79 };
	//P1.Soldier_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_SOLDIER, { x,y }, { 39,40 }, P1.Deploy_UI, false);
	//P1.Tankman_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_TANKMAN, { x,y }, { 39,40 }, P1.Deploy_UI, false);
	//P1.Infiltrator_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_INFILTRATOR, { x,y }, { 39,40 }, P1.Deploy_UI, false);
	//P1.Engineer_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_ENGINEER, { x,y }, { 39,40 }, P1.Deploy_UI, false);
	//P1.War_hound_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_WARHOUND, { x,y }, { 39,40 }, P1.Deploy_UI, false);

	P1.Cast_UI = App->gui->AddUIElement(UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { 0,0 }, { 166,79 }, nullptr, false);
	P1.Cast_UI->rect = { 166,79,166,79 };
	//P1.Missiles_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_CAST_MISSILES, { x,y }, { 39,40 }, player.Cast_UI, false);
	//P1.Cast2_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_CAST_2, { x,y }, { 39,40 }, P1.Cast_UI, false);
	//P1.Cast3_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_CAST_3, { x,y }, { 39,40 }, P1.Cast_UI, false);

	//P1.General_UI = App->gui->AddUIElement(UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { 0,0 }, { w,h }, nullptr, false);
	//P1.Upgrade_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_UPGRADE, { x,y }, { w,h }, P1.General_UI, false);
	//P1.Repair_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_REPAIR, { x,y }, { w,h }, P1.General_UI, false);
	////falta poner los pointer a los datos del edificio seleccionado (ahora esta como "data") 
	//P1.Name_text = App->gui->AddUIElement(UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { x,y }, { w,h }, P1.General_UI, false, { false, false }, "data");
	//P1.Level_text = App->gui->AddUIElement(UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { x,y }, { w,h }, P1.General_UI, false, { false, false }, "data");
	//P1.Health_text = App->gui->AddUIElement(UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { x,y }, { w,h }, P1.General_UI, false, { false, false }, "data");
	//P1.Damage_text = App->gui->AddUIElement(UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { x,y }, { w,h }, P1.General_UI, false, { false, false }, "data");
	//P1.Prod_text = App->gui->AddUIElement(UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { x,y }, { w,h }, P1.General_UI, false, { false, false }, "data");
	//P1.Capacity_text = App->gui->AddUIElement(UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { x,y }, { w,h }, P1.General_UI, false, { false, false }, "data");


	//--- PLAYER 2
	//P2.Health_UI = App->gui->AddUIElement(UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { x,y }, { w,h }, nullptr, true);
	//P2.Gold_UI = App->gui->AddUIElement(UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { x,y }, { w,h }, nullptr, true, { false,false }, "$");
	P2.Main_UI = App->gui->AddUIElement(UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { App->win->width - 166,App->win->height - 79 }, { 166,79 }, nullptr, true);
	P2.Main_UI->rect = { 0,0,166,79 };
	P2.Build_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_GOTO_BUILD, { 12,37 }, { 39,40 }, P2.Main_UI, true);
	P2.Deploy_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_GOTO_DEPLOY, { 63,37 }, { 39,40 }, P2.Main_UI, true);
	P2.Cast_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_GOTO_CAST, { 114,37 }, { 39,40 }, P2.Main_UI, true);

	P2.Build_UI = App->gui->AddUIElement(UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { App->win->width - 166,App->win->height - 79 }, { 166,79 }, nullptr, false);
	P2.Build_UI->rect = { 166,0,166,79 };
	//P2.Def_AOE_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_AOE, { x,y }, { 39,40 } , P2.Build_UI, false);
	//P2.Def_Target_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_TARGET, { x,y }, { 39,40 }, P2.Build_UI, false);
	//P2.Mines_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_MINE, { x,y }, { 39,40 }, player.P2, false);
	//P2.Barracks_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_BARRACKS, { x,y }, { 39,40 }, P2.Build_UI, false);

	P2.Deploy_UI = App->gui->AddUIElement(UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { App->win->width - 166,App->win->height - 79 }, { 166,79 }, nullptr, false);
	P2.Deploy_UI->rect = { 166,0,166,79 };
	//P2.Soldier_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_SOLDIER, { x,y }, { 39,40 }, P2.Deploy_UI, false);
	//P2.Tankman_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_TANKMAN, { x,y }, { 39,40 }, P2.Deploy_UI, false);
	//P2.Infiltrator_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_INFILTRATOR, { x,y }, { 39,40 }, P2.Deploy_UI, false);
	//P2.Engineer_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_ENGINEER, { x,y }, { 39,40 }, P2.Deploy_UI, false);
	//P2.War_hound_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_WARHOUND, { x,y }, { 39,40 }, P2.Deploy_UI, false);

	P2.Cast_UI = App->gui->AddUIElement(UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { App->win->width - 166,App->win->height - 79 }, { 166,79 }, nullptr, false);
	P2.Cast_UI->rect = { 166,0,166,79 };
	//P2.Missiles_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_CAST_MISSILES, { x,y }, { 39,40 }, P2.Cast_UI, false);
	//P2.Cast2_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_CAST_2, { x,y }, { 39,40 }, P2.Cast_UI, false);
	//P2.Cast3_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_CAST_3, { x,y }, { 39,40 }, P2.Cast_UI, false);

	//P2.General_UI = App->gui->AddUIElement(UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { 0,0 }, { w,h }, nullptr, false);
	//P2.Upgrade_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_UPGRADE, { x,y }, { w,h }, P2.General_UI, false);
	//P2.Repair_icon = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_REPAIR, { x,y }, { w,h }, P2.General_UI, false);
	////falta poner los pointer a los datos del edificio seleccionado (ahora esta como "data") 
	//P2.Name_text = App->gui->AddUIElement(UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { x,y }, { w,h }, P2.General_UI, false, { false, false }, "data");
	//P2.Level_text = App->gui->AddUIElement(UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { x,y }, { w,h }, P2.General_UI, false, { false, false }, "data");
	//P2.Health_text = App->gui->AddUIElement(UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { x,y }, { w,h }, P2.General_UI, false, { false, false }, "data");
	//P2.Damage_text = App->gui->AddUIElement(UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { x,y }, { w,h }, player.General_UI, false, { false, false }, "data");
	//P2.Prod_text = App->gui->AddUIElement(UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { x,y }, { w,h }, P2.General_UI, false, { false, false }, "data");
	//P2.Capacity_text = App->gui->AddUIElement(UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { x,y }, { w,h }, P2.General_UI, false, { false, false }, "data");

	// ---------------
	SpawnEntities();

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

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) //return to main_ui player1
	{
		P1.currentUI = CURR_MAIN;
		UpdateVisibility(P1);
	}
	else if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) //return to main_ui player2
	{
		P2.currentUI = CURR_MAIN;
		UpdateVisibility(P2);
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
	//else if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) //View colliders
	//{
	//	App->map->debug = !App->map->debug;
	//	App->entitymanager->draw_path = !App->entitymanager->draw_path;
	//}
	//else if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) //Godmode
	//{
	//	godmode = !godmode;
	//}
	//else if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN) //Framerate Cap On/Off
	//{
	//	App->fpsCapON = !App->fpsCapON;
	//}


	//----------------------------- Just to test the Controllers -----------------------------------
	//-- Player 1
	if (App->input->P1.Controller[JOY_UP] == KEY_REPEAT)
		App->render->camera.y += 150.0*dt;

	if (App->input->P1.Controller[JOY_DOWN] == KEY_REPEAT)
		App->render->camera.y -= 150.0*dt;

	if (App->input->P1.Controller[JOY_RIGHT] == KEY_REPEAT)
		App->render->camera.x -= 150.0*dt;

	if (App->input->P1.Controller[JOY_LEFT] == KEY_REPEAT)
		App->render->camera.x += 150.0*dt;

	//-- Player 2

	if (App->input->P2.Controller[JOY_UP] == KEY_REPEAT)
		App->render->camera.y += 150.0*dt;

	if (App->input->P2.Controller[JOY_DOWN] == KEY_REPEAT)
		App->render->camera.y -= 150.0*dt;

	if (App->input->P2.Controller[JOY_RIGHT] == KEY_REPEAT)
		App->render->camera.x -= 150.0*dt;

	if (App->input->P2.Controller[JOY_LEFT] == KEY_REPEAT)
		App->render->camera.x += 150.0*dt;

	// ------------------------------------------------------------------------------------------------

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

	//----------------------------- Just to test the Controllers -----------------------------------

	if (App->input->P1.Controller[BUTTON_B] == KEY_DOWN)
		ret = false;

	if (App->input->P2.Controller[BUTTON_B] == KEY_DOWN)
		ret = false;
	
	// ------------------------------------------------------------------------------------------------
	
	//--- Update GUI
	list<UI_Element*>::reverse_iterator item = App->gui->UI_elements.rbegin();
	while (item != App->gui->UI_elements.rend())
	{
		if ((*item)->visible == true)
		{
			if (App->gui->CheckMousePos(*item) == true && (*item)->dragging == false && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) != KEY_REPEAT) //hovering
			{
				(*item)->state = UI_Element::State::HOVER;
			}
			if (App->gui->CheckClick(*item) == true && (*item)->state == UI_Element::State::HOVER) //on-click
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
						//App->audio->PlayFx(CLICK);
						if ((*item)->globalpos.second < App->win->height / 2) //is in top of the screen = P1
						{
							DoLogic(P1, *item);
						}
						else if ((*item)->globalpos.second > App->win->height / 2) //is in bottom of the screen = P2
						{
							DoLogic(P2, *item);
						}
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

					App->gui->UpdateChildren();
				}
			}
			else if (App->gui->CheckMousePos(*item) == false && (*item)->state != UI_Element::State::DRAG) //change to idle
			{
				(*item)->state = UI_Element::State::IDLE;
			}
		}
		App->gui->UpdateState(*item);
		item++;
	}

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
	App->entitymanager->DeleteEntities();
	App->map->SwitchMaps(map_names[map]);
	SpawnEntities();

	return true;
}

void Scene::SpawnEntities() //
{
	App->entitymanager->DeleteEntities();
	SpawnEnemies();

	pugi::xml_document	config_file;
	pugi::xml_node		config;

	config = App->LoadConfig(config_file);

	Entity* player = App->entitymanager->AddEntity(Entity::entityType::PLAYER, { 0,0 }, { 0,0 });
	player->Awake(config.child(App->entitymanager->name.data()));
	player->Start();
}


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

void Scene::UpdateVisibility(PlayerUI player) // Update GUI Visibility
{
	switch (player.currentUI)
	{
	case::Scene::CURRENT_UI::CURR_MAIN:
		player.Main_UI->visible = true;
		player.Build_UI->visible = false;
		player.Deploy_UI->visible = false;
		player.Cast_UI->visible = false;
		//player.General_UI->visible = false;
		break;

	case::Scene::CURRENT_UI::CURR_BUILD:
		player.Main_UI->visible = false;
		player.Build_UI->visible = true;
		player.Deploy_UI->visible = false;
		player.Cast_UI->visible = false;
		//player.General_UI->visible = false;
		break;

	case::Scene::CURRENT_UI::CURR_DEPLOY:
		player.Main_UI->visible = false;
		player.Build_UI->visible = false;
		player.Deploy_UI->visible = true;
		player.Cast_UI->visible = false;
		//player.General_UI->visible = false;
		break;

	case::Scene::CURRENT_UI::CURR_CAST:
		player.Main_UI->visible = false;
		player.Build_UI->visible = false;
		player.Deploy_UI->visible = false;
		player.Cast_UI->visible = true;
		//player.General_UI->visible = false;
		break;

	case::Scene::CURRENT_UI::CURR_GENERAL:
		//	player.Main_UI->visible = false;
		//	player.Build_UI->visible = false;
		//	player.Deploy_UI->visible = false;
		//	player.Cast_UI->visible = false;
		//	player.General_UI->visible = true;
		break;
	}
	App->gui->UpdateChildren();
}

void Scene::DoLogic(PlayerUI player, UI_Element* data)
{
	switch (data->action)
	{
	case::UI_Element::Action::ACT_GOTO_MAIN:
		player.currentUI = CURR_MAIN;
		UpdateVisibility(player);
		break;

	case::UI_Element::Action::ACT_GOTO_BUILD:
		player.currentUI = CURR_BUILD;
		UpdateVisibility(player);
		break;

	case::UI_Element::Action::ACT_GOTO_DEPLOY:
		player.currentUI = CURR_DEPLOY;
		UpdateVisibility(player);
		break;

	case::UI_Element::Action::ACT_GOTO_CAST:
		player.currentUI = CURR_CAST;
		UpdateVisibility(player);
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