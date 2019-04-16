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
#include "Building.h"
#include "Troop.h"

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

	pause_soviet = config.child("pause_menu_soviet").attribute("file").as_string("");
	pause_alied = config.child("pause_menu_alied").attribute("file").as_string("");

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	bool ret = false;

	App->map->Load(map_names.front()->data());
	//current_track = App->audio->tracks_path.front();
	//App->audio->PlayMusic(PATH(App->audio->folder_music.data(), current_track.data()));
	App->render->zoom = 0.77;

	// Variables init
	currentMap = 0;
	pause = false;
	godmode = false;
	to_end = false;
	change = false;
	endgame = false;
	App->player1->isPlayer1 = true;
	App->player2->isPlayer1 = false;

	App->player1->currentUI = Player::CURRENT_UI::NONE;
	App->player2->currentUI = Player::CURRENT_UI::NONE;

	App->map->debug_tex = App->tex->Load("maps/meta.png");

	App->player1->LiveBar = { 51, 18 , 348, 19 }; //LiveBar for player1
	App->player2->LiveBar = { 1232, 921 , 348, 19 }; //LiveBar for player2


	//walkability map
	Wmap = NULL;
	int w, h;
	if (App->map->CreateWalkabilityMap(w, h, &Wmap))
	{
		App->pathfinding->SetMap(w, h, Wmap);
		LOG("Create walkability map");
	}

	

	//--------- INIT BUILD ZONE LIMITS ------------- (falta cambiar con valores de las build Zones
	App->player1->x_limits_player1 = { 20,60 };
	App->player1->y_limits_player1 = { 11,33 };

	App->player2->x_limits_player2 = { 20,60 };
	App->player2->y_limits_player2 = { 56,78 };

		//spritesheet123 = App->tex->Load("textures/Buildings.png");
	App->map->imagemap = App->tex->Load("textures/map3.png");
	


	//--------- CREATE MAIN BUILDINGS -------------// (falta cambiar posicion)
	//--- PLAYER 1
	/*App->entitymanager->AddEntity(true, Entity::entityType::TOWNHALL, { 50,50 });
	App->entitymanager->AddEntity(true, Entity::entityType::MAIN_DEFENSE, { 50,50 });
	App->entitymanager->AddEntity(true, Entity::entityType::COMMAND_CENTER, { 50,50 });
	App->entitymanager->AddEntity(true, Entity::entityType::WALLS, { 50,50 });
	App->entitymanager->AddEntity(true, Entity::entityType::DEFENSE_TARGET, { 50,50 });

	//--- PLAYER 2
	App->entitymanager->AddEntity(false, Entity::entityType::TOWNHALL, { 50,50 });
	App->entitymanager->AddEntity(false, Entity::entityType::MAIN_DEFENSE, { 50,50 });
	App->entitymanager->AddEntity(false, Entity::entityType::COMMAND_CENTER, { 50,50 });
	App->entitymanager->AddEntity(false, Entity::entityType::WALLS, { 50,50 });
	App->entitymanager->AddEntity(false, Entity::entityType::SOLDIER, { 5000,50 });
	App->entitymanager->AddEntity(false, Entity::entityType::SOLDIER, { 350,400 });*/

	string track = App->audio->folder_music + "/StartSong.ogg"; 
	App->audio->PlayMusic(track.c_str()); 

	//--------- CREATE GUI -----------//  (Falta poner position y size)
	ui_timer = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, {800, 0 }, { 0, 0 }, nullptr, true, { false, false }, "Timer: 0s");
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
	App->player1->Def_AOE_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_AOE, { 68, 55 }, { 85, 81 }, App->player1->Build_UI, false);
	App->player1->Def_Target_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_TARGET, { 171, 55 }, { 85, 81 }, App->player1->Build_UI, false);
	App->player1->Mines_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_MINE, { 274, 55 }, { 85,81 }, App->player1->Build_UI, false);
	App->player1->Barracks_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_BARRACKS, { 375, 55 }, { 85, 81 }, App->player1->Build_UI, false);

	App->player1->Deploy_UI = App->gui->AddUIElement(true, UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { 0,0 }, { 566, 163 }, nullptr, false);
	App->player1->Deploy_UI->rect = { 569, 411, 566, 163 };
	App->player1->Soldier_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_SOLDIER, { 68, 55 }, { 85, 81 }, App->player1->Deploy_UI, false);
	App->player1->Tankman_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_TANKMAN, { 171, 55 }, { 85, 81 }, App->player1->Deploy_UI, false);
	App->player1->Engineer_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_ENGINEER, { 274, 55 }, { 85, 81 }, App->player1->Deploy_UI, false);
	App->player1->War_hound_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_WARHOUND, { 377, 55 }, { 85, 81 }, App->player1->Deploy_UI, false);
	App->player1->Infiltrator_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_INFILTRATOR, { 480, 55 }, { 85, 81 }, App->player1->Deploy_UI, false);


	App->player1->Cast_UI = App->gui->AddUIElement(true, UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { 0,0 }, { 166,79 }, nullptr, false);
	App->player1->Cast_UI->rect = { 0, 573, 566, 163 };
	App->player1->Cast2_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_CAST_TANKS, { 171 ,55 }, { 85, 81 }, App->player1->Cast_UI, false);
	App->player1->Cast3_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_CAST_INVULNERABILITY, { 273, 55 }, { 85, 81 }, App->player1->Cast_UI, false);
	App->player1->Missiles_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_CAST_MISSILES, { 375, 55 }, { 85, 81 }, App->player1->Cast_UI, false);

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

	App->player1->Gold_UI = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 470, 19 }, { 0, 0 }, nullptr, true, { false,false }, "$ 0000");
	App->player1->Gold_UI->color = { 255,255,0,255 };

	// ------ PAUSE MENU ------
	App->player1->Pause_UI = App->gui->AddUIElement(true, UI_Element::UI_type::TEXTURE, UI_Element::Action::NONE, { 0, 0 }, { App->win->width, App->win->height }, nullptr, false);
	App->player1->Pause_UI->texture = App->tex->Load(pause_soviet.data());
	App->player1->Pause_UI->rect = { 0, 0, App->win->width, App->win->height };

	App->player1->Abort_Button = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ABORT_PAUSE, { 1291 ,868 }, { 301,59 }, App->player1->Pause_UI, false);
	App->player1->Abort_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 80, 18 }, { 0, 0 }, App->player1->Abort_Button, false, { false, false });
	App->player1->Abort_text->label = App->player1->abort_label;
	App->player1->Abort_text->color = { 255,255,9,255 };


	App->player1->Settings_Button = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::SETTINGS_PAUSE, { 1291 ,565 }, { 301,59 }, App->player1->Pause_UI, false);
	App->player1->Settings_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 120, 18 }, { 0, 0 }, App->player1->Settings_Button, false, { false, false });
	App->player1->Settings_text->label = App->player1->settings_label;
	App->player1->Settings_text->color = { 255,255,9,255 };

	App->player1->Resume_Button = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::RESUME_PAUSE, { 1291 ,498 }, { 301,59 }, App->player1->Pause_UI, false);
	App->player1->Resume_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 80, 18 }, { 0, 0 }, App->player1->Resume_Button, false, { false, false });
	App->player1->Resume_text->label = App->player1->resume_label;
	App->player1->Resume_text->color = { 255,255,9,255 };

	//------ Settings Pause MENU ------
	App->player1->Settings_UI = App->gui->AddUIElement(true, UI_Element::UI_type::TEXTURE, UI_Element::Action::NONE, { 0, 0 }, { App->win->width , App->win->height }, nullptr, false);
	App->player1->Settings_UI->texture = App->tex->Load(pause_soviet.data());
	App->player1->Settings_UI->rect = { 0, 0, 0, App->win->height };

	App->player1->Music_Settings = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::MUSIC_VOLUME, { 100 ,100 }, { 301,59 }, App->player1->Settings_UI, false);
	App->player1->Music_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 80, 18 }, { 0, 0 }, App->player1->Music_Settings, false, { false, false });
	App->player1->Music_text->label = App->player1->Music_label;
	App->player1->Music_text->color = { 255,255,9,255 };

	App->player1->Music_Slider = App->gui->AddUIElement(true, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 50 , 100 }, { 651, 59 }, App->player1->Music_Settings, false);
	App->player1->Music_Slider->rect = {1483, 770, 651, 59 };
	App->player1->Music_Slider_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 640, 18 }, { 0, 0 }, App->player1->Music_Slider, false, { false, false });
	App->player1->Music_Slider_text->label = App->player1->Music_Slider_label;
	App->player1->Music_Slider_text->color = { 255,255,9,255 };
	App->player1->Music_Slider_Button = App->gui->AddUIElement(true, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 325 , 3 }, { 29, 55 }, App->player1->Music_Slider, false);
	App->player1->Music_Slider_Button->rect = { 2135, 773, 29, 55 };


	App->player1->FX_Settings = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::FX_VOLUME, { 100 ,400 }, { 301,59 }, App->player1->Settings_UI, false);
	App->player1->FX_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 100, 18 }, { 0, 0 }, App->player1->FX_Settings, false, { false, false });
	App->player1->FX_text->label = App->player1->FX_label;
	App->player1->FX_text->color = { 255,255,9,255 };

	App->player1->FX_Slider = App->gui->AddUIElement(true, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 50 , 100 }, { 651, 59 }, App->player1->FX_Settings, false);
	App->player1->FX_Slider->rect = { 1483, 770, 651, 59 };
	App->player1->FX_Slider_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 640, 18 }, { 0, 0 }, App->player1->FX_Slider, false, { false, false });
	App->player1->FX_Slider_text->label = App->player1->FX_Slider_label;
	App->player1->FX_Slider_text->color = { 255,255,9,255 };
	App->player1->FX_Slider_Button = App->gui->AddUIElement(true, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 325 , 3 }, { 29, 55 }, App->player1->FX_Slider, false);
	App->player1->FX_Slider_Button->rect = { 2135, 773, 29, 55 };

	// ABORT MISSION MENU
	App->player1->Abort_UI = App->gui->AddUIElement(true, UI_Element::UI_type::TEXTURE, UI_Element::Action::NONE, { 0, 0 }, { App->win->width , App->win->height }, nullptr, false);
	App->player1->Abort_UI->texture = App->tex->Load(pause_soviet.data());
	App->player1->Abort_UI->rect = { 0, 0, 0, App->win->height };

	App->player1->Restart = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::RESTART, { 1291 ,565 }, { 301,59 }, App->player1->Abort_UI, false);
	App->player1->Restart_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 80, 18 }, { 0, 0 }, App->player1->Restart, false, { false, false });
	App->player1->Restart_text->label = App->player1->Restart_label;
	App->player1->Restart_text->color = { 255,255,9,255 };

	App->player1->Abort_Message = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { -850, -100 }, { 0, 0 }, App->player1->Restart, false, { false, false });
	App->player1->Abort_Message->label = App->player1->Abort_message_label;
	App->player1->Abort_Message->color = { 255,255,9,255 };

	App->player1->Quit = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::SURRENDER, { 1291 ,498 }, { 301,59 }, App->player1->Abort_UI, false);
	App->player1->Quit_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 110, 18 }, { 0, 0 }, App->player1->Quit, false, { false, false });
	App->player1->Quit_text->label = App->player1->Quit_label;
	App->player1->Quit_text->color = { 255,255,9,255 };


	//--- PLAYER 2
	//App->player2->Health_UI = App->gui->AddUIElement(false, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { x,y }, { w,h }, nullptr, true);
	//App->player2->Gold_UI = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { x,y }, { w,h }, nullptr, true, { false,false }, "$");
	App->player2->Main_UI = App->gui->AddUIElement(false, UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { App->win->width - 145 ,App->win->height + 122 }, { 566,163 }, nullptr, true);
	App->player2->Main_UI->rect = { 0, 408, 566, 163 };
	App->player2->Build_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_GOTO_BUILD, { 275,55 }, { 80, 81 }, App->player2->Main_UI, true);
	App->player2->Deploy_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_GOTO_DEPLOY, { 374,55 }, { 80, 81 }, App->player2->Main_UI, true);
	App->player2->Cast_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_GOTO_CAST, { 483,55 }, { 80,81 }, App->player2->Main_UI, true);


	App->player2->Build_UI = App->gui->AddUIElement(false, UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { App->win->width - 145 ,App->win->height + 123 }, { 566, 163 }, nullptr, false);
	App->player2->Build_UI->rect = { 569, 246, 566, 163 };
	App->player2->Def_AOE_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_AOE, { 68, 55 }, { 85, 81 }, App->player2->Build_UI, false);
	App->player2->Def_Target_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_TARGET, { 171, 55 }, { 85, 81 }, App->player2->Build_UI, false);
	App->player2->Mines_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_MINE, { 274, 55 }, { 85,81 }, App->player2->Build_UI, false);
	App->player2->Barracks_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_BARRACKS, { 375, 55 }, { 85, 81 }, App->player2->Build_UI, false);

	App->player2->Deploy_UI = App->gui->AddUIElement(false, UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { App->win->width - 145 ,App->win->height + 123 }, { 566, 163 }, nullptr, false);
	App->player2->Deploy_UI->rect = { 569, 411, 566, 163 };
	App->player2->Soldier_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_SOLDIER, { 68, 55 }, { 85, 81 }, App->player2->Deploy_UI, false);
	App->player2->Tankman_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_TANKMAN, { 171, 55 }, { 85, 81 }, App->player2->Deploy_UI, false);
	App->player2->Engineer_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_ENGINEER, { 274, 55 }, { 85, 81 }, App->player2->Deploy_UI, false);
	App->player2->War_hound_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_WARHOUND, { 377, 55 }, { 85, 81 }, App->player2->Deploy_UI, false);
	App->player2->Infiltrator_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_INFILTRATOR, { 480, 55 }, { 85, 81 }, App->player2->Deploy_UI, false);

	App->player2->Cast_UI = App->gui->AddUIElement(false, UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { App->win->width - 145 ,App->win->height + 123 }, { 166,79 }, nullptr, false);
	App->player2->Cast_UI->rect = { 0, 573, 566, 163 };
	App->player2->Cast2_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_CAST_TANKS, { 171 ,55 }, { 85, 81 }, App->player2->Cast_UI, false);
	App->player2->Cast3_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_CAST_INVULNERABILITY, { 273, 55 }, { 85, 81 }, App->player2->Cast_UI, false);
	App->player2->Missiles_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_CAST_MISSILES, { 375, 55 }, { 85, 81 }, App->player2->Cast_UI, false);

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

	App->player2->Gold_UI = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 2000, 1191 }, { 0, 0 }, nullptr, true, { false,false }, "$ 0000");
	App->player2->Gold_UI->color = { 255,255,0,255 };

	App->player2->Pause_UI = App->gui->AddUIElement(false, UI_Element::UI_type::TEXTURE, UI_Element::Action::NONE, { 0, 0 }, { App->win->width, App->win->height }, nullptr, false);
	App->player2->Pause_UI->texture = App->tex->Load(pause_alied.data());
	App->player2->Pause_UI->rect = { 0, 0, App->win->width, App->win->height };

	App->player2->Abort_Button = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ABORT_PAUSE, { 1291 ,868 }, { 301,59 }, App->player2->Pause_UI, false);
	App->player2->Abort_text = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 80, 18 }, { 0, 0 }, App->player2->Abort_Button, false, { false, false });
	App->player2->Abort_text->label = App->player2->abort_label;
	App->player2->Abort_text->color = { 255,255,9,255 };

	App->player2->Settings_Button = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::SETTINGS_PAUSE, { 1291 ,565 }, { 301,59 }, App->player2->Pause_UI, false);
	App->player2->Settings_text = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 120, 18 }, { 0, 0 }, App->player2->Settings_Button, false, { false, false });
	App->player2->Settings_text->label = App->player2->settings_label;
	App->player2->Settings_text->color = { 255,255,9,255 };

	/*App->player1->Settings_Button = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::SETTINGS_PAUSE, { 1291 ,565 }, { 301,59 }, App->player1->Pause_UI, false);
	App->player1->Settings_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 120, 18 }, { 0, 0 }, App->player1->Settings_Button, false, { false, false });
	App->player1->Settings_text->label = App->player1->settings_label;
	App->player1->Settings_text->color = { 255,255,9,255 };*/

	App->player2->Resume_Button = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::RESUME_PAUSE, { 1291 ,498 }, { 301,59 }, App->player2->Pause_UI, false);
	App->player2->Resume_text = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 80, 18 }, { 0, 0 }, App->player2->Resume_Button, false, { false, false });
	App->player2->Resume_text->label = App->player2->resume_label;
	App->player2->Resume_text->color = { 255,255,9,255 };


	//------ Settings Pause MENU ------
	App->player2->Settings_UI = App->gui->AddUIElement(false, UI_Element::UI_type::TEXTURE, UI_Element::Action::NONE, { 0, 0 }, { App->win->width - 400, App->win->height }, nullptr, false);
	App->player2->Settings_UI->texture = App->tex->Load(pause_alied.data());
	App->player2->Settings_UI->rect = { 0, 0, 0, App->win->height };

	App->player2->Music_Settings = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::MUSIC_VOLUME, { 100 ,100 }, { 301,59 }, App->player2->Settings_UI, false);
	App->player2->Music_text = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 80, 18 }, { 0, 0 }, App->player2->Music_Settings, false, { false, false });
	App->player2->Music_text->label = App->player2->Music_label;
	App->player2->Music_text->color = { 255,255,9,255 };

	App->player2->Music_Slider = App->gui->AddUIElement(false, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 50 , 100 }, { 651, 59 }, App->player2->Music_Settings, false);
	App->player2->Music_Slider->rect = { 1483, 709, 651, 59 };
	App->player2->Music_Slider_text = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 640, 18 }, { 0, 0 }, App->player2->Music_Slider, false, { false, false });
	App->player2->Music_Slider_text->label = App->player2->Music_Slider_label;
	App->player2->Music_Slider_text->color = { 255,255,9,255 };
	App->player2->Music_Slider_Button = App->gui->AddUIElement(false, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 325 , 3 }, { 29, 55 }, App->player2->Music_Slider, false);
	App->player2->Music_Slider_Button->rect = { 2135, 712, 29, 55 };

	App->player2->FX_Settings = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::FX_VOLUME, { 100 ,400 }, { 301,59 }, App->player2->Settings_UI, false);
	App->player2->FX_text = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 100, 18 }, { 0, 0 }, App->player2->FX_Settings, false, { false, false });
	App->player2->FX_text->label = App->player2->FX_label;
	App->player2->FX_text->color = { 255,255,9,255 };

	App->player2->FX_Slider = App->gui->AddUIElement(false, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 50 , 100 }, { 651, 59 }, App->player2->FX_Settings, false);
	App->player2->FX_Slider->rect = { 1483, 709, 651, 59 };
	App->player2->FX_Slider_text = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 640, 18 }, { 0, 0 }, App->player2->FX_Slider, false, { false, false });
	App->player2->FX_Slider_text->label = App->player2->FX_Slider_label;
	App->player2->FX_Slider_text->color = { 255,255,9,255 };
	App->player2->FX_Slider_Button = App->gui->AddUIElement(false, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 325 , 3 }, { 29, 55 }, App->player2->FX_Slider, false);
	App->player2->FX_Slider_Button->rect = { 2135, 712, 29, 55 };

	// ABORT MISSION MENU
	App->player2->Abort_UI = App->gui->AddUIElement(false, UI_Element::UI_type::TEXTURE, UI_Element::Action::NONE, { 0, 0 }, { App->win->width , App->win->height }, nullptr, false);
	App->player2->Abort_UI->texture = App->tex->Load(pause_alied.data());
	App->player2->Abort_UI->rect = { 0, 0, 0, App->win->height };

	App->player2->Restart = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::RESTART, { 1291 ,565 }, { 301,59 }, App->player2->Abort_UI, false);
	App->player2->Restart_text = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 80, 18 }, { 0, 0 }, App->player2->Restart, false, { false, false });
	App->player2->Restart_text->label = App->player2->Restart_label;
	App->player2->Restart_text->color = { 255,255,9,255 };

	App->player2->Abort_Message = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { -850, -100 }, { 0, 0 }, App->player2->Restart, false, { false, false });
	App->player2->Abort_Message->label = App->player2->Abort_message_label;
	App->player2->Abort_Message->color = { 255,255,9,255 };

	App->player2->Quit = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::SURRENDER, { 1291 ,498 }, { 301,59 }, App->player2->Abort_UI, false);
	App->player2->Quit_text = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 110, 18 }, { 0, 0 }, App->player2->Quit, false, { false, false });
	App->player2->Quit_text->label = App->player2->Quit_label;
	App->player2->Quit_text->color = { 255,255,9,255 };




	// --- CURSORS
	App->player1->currentTile.first = 13;
	App->player1->currentTile.second = 0;

	App->player2->currentTile.first = 39;
	App->player2->currentTile.second = 69;

	if (App->player1->gamepad.Connected == false)
		LOG("---Player 1 Gamepad not Connected");

	if (App->player2->gamepad.Connected == false)
		LOG("---Player 2 Gamepad not Connected");

	if (cursor_tex == nullptr)
		cursor_tex = App->tex->Load("textures/Cursors.png");

	//-----------
	SpawnEntities();

	App->font->font_iterator = App->font->fonts.begin();

	//Map load 
	App->map->LoadTileList();

	worldminutes = 0;
	worldseconds = 0;
	reference_active = true;
	change_font_size = false;

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

	if (!pause)
	{
		// Gold Player 1 ------------
		list<Building*>::iterator item = App->player1->buildings.begin();
		for (item; item != App->player1->buildings.end(); ++item)
		{
			if ((*item)->type == Entity::entityType::TOWNHALL || (*item)->type == Entity::entityType::MINES)
			{
				App->player1->gold += (*item)->production;
			}
		}
		// This updates the gold every second. This has to be in the if above: if ((*item)->type == Entity::entityType::TOWNHALL || (*item)->type == Entity::entityType::MINES)
		if ((int)world_clock.ReadSec() > App->player1->time_iterator)
		{
			App->player1->gold += 100;
			App->player1->time_iterator = (int)world_clock.ReadSec();
		}



		// Gold Player 2 -----------
		item = App->player2->buildings.begin();
		for (item; item != App->player2->buildings.end(); ++item)
		{
			if ((*item)->type == Entity::entityType::TOWNHALL || (*item)->type == Entity::entityType::MINES)
			{
				App->player2->gold += (*item)->production;
			}
		}
		// This updates the gold every second. This has to be in the if above: if ((*item)->type == Entity::entityType::TOWNHALL || (*item)->type == Entity::entityType::MINES)
		if ((int)world_clock.ReadSec() > App->player2->time_iterator)
		{
			App->player2->gold += 100;
			App->player2->time_iterator = (int)world_clock.ReadSec();
		}
		//----
	}
	


	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	BROFILER_CATEGORY("Scene Update", Profiler::Color::DarkOrange);
	int x, y;
	App->input->GetMousePosition(x, y);


	
	// Player 1 -> number / Player2 -> letter
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) 
	{
		App->entitymanager->AddEntity(true, Entity::entityType::TOWNHALL, { x,y });
	}
	else if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN)
	{
		App->entitymanager->AddEntity(false, Entity::entityType::TOWNHALL, { x,y });
	}
	else if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		App->entitymanager->AddEntity(true, Entity::entityType::MAIN_DEFENSE, { x,y });
	}
	else if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
	{
		App->entitymanager->AddEntity(false, Entity::entityType::MAIN_DEFENSE, { x,y });
	}
	else if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		App->entitymanager->AddEntity(true, Entity::entityType::COMMAND_CENTER, { x,y });
	}
	else if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
	{
		App->entitymanager->AddEntity(false, Entity::entityType::COMMAND_CENTER, { x,y });
	}
	else if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
	{
		App->entitymanager->AddEntity(true, Entity::entityType::BARRACKS, { x,y });
	}
	else if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
	{
		App->entitymanager->AddEntity(false, Entity::entityType::BARRACKS, { x,y });
	}
	else if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN)
	{
		App->entitymanager->AddEntity(true, Entity::entityType::DEFENSE_AOE, { x,y });
	}
	else if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN)
	{
		App->entitymanager->AddEntity(false, Entity::entityType::DEFENSE_AOE, { x,y });
	}
	else if (App->input->GetKey(SDL_SCANCODE_6) == KEY_DOWN)
	{
		App->entitymanager->AddEntity(true, Entity::entityType::DEFENSE_TARGET, { x,y });
	}
	else if (App->input->GetKey(SDL_SCANCODE_Y) == KEY_DOWN)
	{
		App->entitymanager->AddEntity(false, Entity::entityType::DEFENSE_TARGET, { x,y });
	}
	else if (App->input->GetKey(SDL_SCANCODE_7) == KEY_DOWN)
	{
		App->entitymanager->AddEntity(true, Entity::entityType::MINES, { x,y });
	}
	else if (App->input->GetKey(SDL_SCANCODE_U) == KEY_DOWN)
	{
		App->entitymanager->AddEntity(false, Entity::entityType::MINES, { x,y });
	}
	else if (App->input->GetKey(SDL_SCANCODE_8) == KEY_DOWN)
	{
		App->entitymanager->AddEntity(true, Entity::entityType::SOLDIER, { x,y });
	}
	else if (App->input->GetKey(SDL_SCANCODE_I) == KEY_DOWN)
	{
		App->entitymanager->AddEntity(false, Entity::entityType::SOLDIER, { x,y });
	}
	else if (App->input->GetKey(SDL_SCANCODE_0) == KEY_DOWN)
	{
		App->entitymanager->AddEntity(true, Entity::entityType::WALLS, { x,y });
	}
	else if (App->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN)
	{
		App->entitymanager->AddEntity(false, Entity::entityType::WALLS, { x,y });
	}
	else if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) //Movecamera Up
	{
		App->render->camera.y -= 10;
	}
	else if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) //Movecamera Down
	{
		App->render->camera.y += 10;
	}
	else if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) //Movecamera Left
	{
		App->render->camera.x += 10;
	}
	else if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) //Movecamera Right
	{
		App->render->camera.x -= 10;
	}

	

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
	if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN) //Debug UI
	{
		App->gui->UI_Debug = !App->gui->UI_Debug;
	}
	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) //View colliders
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

	App->map->DrawWakability(dt);
	App->entitymanager->Draw(dt);

	App->gui->Draw();

	//DRAW LIVE BARS 
	if (!pause)
	{
		DrawLiveBar(App->player1);
		DrawLiveBar(App->player2);
	}
	

	

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
		}
		else if ((*item) == App->player1->Gold_UI) //gold
		{
			sprintf_s(current_p1_gold, "$ %u", App->player1->gold);
			(*item)->label = current_p1_gold;
		}
		else if ((*item) == App->player1->Music_Slider_text) //Music VOlume
		{
			sprintf_s(App->player1->Music_Slider_label, "%u", App->audio->musicVolume);
			(*item)->label = App->player1->Music_Slider_label;
		}
		else if ((*item) == App->player1->FX_Slider_text) //FX Volume
		{
			sprintf_s(App->player1->FX_Slider_label, "%u", App->audio->sfxVolume);
			(*item)->label = App->player1->FX_Slider_label;
		}
		else if ((*item) == App->player1->Music_Slider_Button) //MUSIC SLIDER POS
		{
			App->player1->Music_Slider_Button->position.first = (550 * App->audio->musicVolume) / 100;
		}
		else if ((*item) == App->player1->FX_Slider_Button) //FX SLIDER POS
		{
			App->player1->FX_Slider_Button->position.first = (550 * App->audio->sfxVolume) / 100;
		}
		

		App->gui->UpdateState(*item);
		item++;
	}

	//--- Update Player 2 GUI
	item = App->player2->UI_elements.rbegin();
	while (item != App->player2->UI_elements.rend())
	{
		if ((*item) == App->player2->Gold_UI) //gold
		{
			sprintf_s(current_p2_gold, "$ %u", App->player2->gold);
			(*item)->label = current_p2_gold;
		}
		else if ((*item) == App->player2->Music_Slider_text) //Music VOlume
		{
			sprintf_s(App->player2->Music_Slider_label, "%u", App->audio->musicVolume);
			(*item)->label = App->player2->Music_Slider_label;
		}
		else if ((*item) == App->player2->FX_Slider_text) //FX Volume
		{
			sprintf_s(App->player2->FX_Slider_label, "%u", App->audio->sfxVolume);
			(*item)->label = App->player2->FX_Slider_label;
		}
		else if ((*item) == App->player2->Music_Slider_Button) //MUSIC SLIDER POS
		{
			App->player2->Music_Slider_Button->position.first = (550 * App->audio->musicVolume) / 100;
		}
		else if ((*item) == App->player2->FX_Slider_Button) //FX SLIDER POS
		{
			App->player2->FX_Slider_Button->position.first = (550 * App->audio->sfxVolume) / 100;
		}
		App->gui->UpdateState(*item);
		item++;
	}


	App->gui->UpdateChildren();


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

	App->tex->UnLoad(cursor_tex);
	App->tex->UnLoad(App->map->imagemap);
	App->tex->UnLoad(App->map->debug_tex);

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
	App->player1->TownHall = App->entitymanager->AddEntity(true, Entity::entityType::TOWNHALL, { 100, 700 });
}


void Scene::DrawLiveBar(Player* player)
{
	player->LiveBar.w = (348 * player->live) / 2000; // (maximum rect width * town hall live) / MAX town hall live

	if (player->live >= 1500)
		App->render->DrawQuad(player->LiveBar, 0, 255, 0, 255, true, false);
	else if (player->live >= 750 && player->live <= 1500)
		App->render->DrawQuad(player->LiveBar, 255, 150, 0, 255, true, false);
	else
		App->render->DrawQuad(player->LiveBar, 255, 0, 0, 255, true, false);
}

void Scene::Victorious()
{
	if (App->player1->isPlayer1)
	{
		App->scene->pause = true;
		SDL_Rect r = { 0,0,App->win->width,App->win->height };
		App->render->DrawQuad({ 0, 0, (int)App->win->width + 500, (int)App->win->height + 150 }, 0, 0, 0, 150, true, false);
		SDL_RenderCopy(App->render->renderer, App->gui->allied_win_tex, NULL, &r);
		pausetimer = true;
		world_seconds.Stop();

		//rematch_button = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::REMATCH, { 1273, 432 }, { 187, 37 }, nullptr, true);
		//rematch_button_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 155, 30 }, { 0, 0 }, rematch_button, true, { false, false });
		//rematch_button->label = rematch_button->label;
		//rematch_button->color = { 255, 255, 9, 255 };

		//return_mainmenu = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::RETURN_MAINMENU, { 1273, 532 }, { 187, 37 }, nullptr, true);
		//return_mainmenu_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 155, 30 }, { 0, 0 }, return_mainmenu, true, { false, false });
		//return_mainmenu->label = return_mainmenu->label;
		//return_mainmenu->color = { 255, 255, 9, 255 };

		endgame = true;
	}
	else if (!App->player2->isPlayer1)
	{
		App->scene->pause = true;
		SDL_Rect r = { 0,0,App->win->width,App->win->height };
		App->render->DrawQuad({ 0, 0, (int)App->win->width + 500, (int)App->win->height + 150 }, 0, 0, 0, 150, true, false);
		SDL_RenderCopy(App->render->renderer, App->gui->soviet_win_tex, NULL, &r);
		pausetimer = true;
		world_seconds.Stop();

		//rematch_button = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::REMATCH, { 1273, 432 }, { 187, 37 }, nullptr, true);
		//rematch_button_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 155, 30 }, { 0, 0 }, rematch_button, true, { false, false });
		//rematch_button->label = rematch_button->label;
		//rematch_button->color = { 255, 255, 9, 255 };

		//return_mainmenu = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::RETURN_MAINMENU, { 1273, 532 }, { 187, 37 }, nullptr, true);
		//return_mainmenu_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 155, 30 }, { 0, 0 }, return_mainmenu, true, { false, false });
		//return_mainmenu->label = return_mainmenu->label;
		//return_mainmenu->color = { 255, 255, 9, 255 };

		endgame = true;
	}

}

void Scene::ResetGame()
{
	App->scene->pause = false;

	pausetimer = false;
	world_seconds.Start();

	App->scene->CleanUp();
	App->entitymanager->CleanUp();
	App->player2->CleanUp();
	App->player1->CleanUp();
	App->gui->CleanUp();
	App->map->CleanUp();
	App->font->CleanUp();

	App->font->Start();
	App->map->Start();
	App->gui->Start();
	App->player1->Start();
	App->player2->Start();
	App->scene->Start();
}