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
#include "Walls.h"

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

	allied_win_name = config.child("allied").attribute("file").as_string("");
	soviet_win_name = config.child("soviet").attribute("file").as_string("");
	draw_name = config.child("draw").attribute("file").as_string("");

	
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

	allied_win_tex = App->tex->Load(allied_win_name.data());
	soviet_win_tex = App->tex->Load(soviet_win_name.data());
	draw_tex = App->tex->Load(draw_name.data());

	pause_soviet_texture = App->tex->Load(pause_soviet.data());
	pause_alied_texture = App->tex->Load(pause_alied.data());

	// Variables init
	currentMap = 0;
	pause = false;
	godmode = true;
	to_end = false;
	change = false;
	endgame = false;
	/*App->player1->isPlayer1 = true;
	App->player2->isPlayer1 = false;
*/
	App->player1->currentUI = Player::CURRENT_UI::CURR_MAIN;
	App->player2->currentUI = Player::CURRENT_UI::CURR_MAIN;

	App->map->debug_tex = App->tex->Load("maps/meta.png");

	trees_tex = App->tex->Load("animation/Trees_anim.png"); 
	bush_rect = { 276 ,0, 92, 78 }; 
	fit_tree =  { 184 ,0, 92, 78 };
	wide_tree = { 92 ,0, 92, 78 };
	tall_tree = { 0 ,0, 92, 78 };

	flags_tex = App->tex->Load("animation/Flags_anim.png"); 

	App->map->allied_flag_anim = App->map->allied_flag_anim->LoadAnimation("animation/Flags.tmx", "flag_allied");
	App->map->soviet_flag_anim = App->map->soviet_flag_anim->LoadAnimation("animation/Flags.tmx", "flag_soviet");

	App->map->allied_flag_anim->speed = 6;
	App->map->soviet_flag_anim->speed = 6;


	explosion_tex = App->tex->Load("animation/explosion_anim.png");
	App->map->explosion_anim = App->map->explosion_anim->LoadAnimation("animation/explosion.tmx", "animation");


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

	debug_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 0, 200 }, { 0, 0 }, nullptr, true, { false, false });
	debug_text->label = debug_label;
	debug_text->color = { 255, 255, 255, 255};
	debug_text->visible = false;

	//--------- INIT BUILD ZONE LIMITS ------------- (falta cambiar con valores de las build Zones
	App->player1->x_limits_player1 = { 20,60 };
	App->player1->y_limits_player1 = { 20,33 };

	App->player2->x_limits_player2 = { 20,60 };
	App->player2->y_limits_player2 = { 56,69 };

		//spritesheet123 = App->tex->Load("textures/Buildings.png");
	App->map->imagemap = App->tex->Load("textures/map3.png");
	

	

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

	App->player1->Y_to_Main = App->gui->AddUIElement(true, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 5 , 70 }, { 39, 39 }, nullptr, true);
	App->player1->Y_to_Main->rect = { 1289, 373, 39, 39 };

	App->player1->Y_to_Main2 = App->gui->AddUIElement(true, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 590 , 70 }, { 39, 39 }, nullptr, true);
	App->player1->Y_to_Main2->rect = { 1289, 373, 39, 39 };

	App->player1->SelectBuilding = App->gui->AddUIElement(true, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 630 , 30 }, { 39, 39 }, nullptr, true);
	App->player1->SelectBuilding->rect = { 1226, 488, 45, 107 };
	App->player1->In_SelectBuilding = App->gui->AddUIElement(true, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 630 , 30 }, { 39, 39 }, nullptr, false);
	App->player1->In_SelectBuilding->rect = { 1326, 488, 45, 107 };


	App->player1->Build_UI = App->gui->AddUIElement(true, UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { 0,0 }, { 566, 163 }, nullptr, false);
	App->player1->Build_UI->rect = { 569, 246, 566, 163 };
	App->player1->Def_AOE_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_AOE, { 68, 55 }, { 85, 81 }, App->player1->Build_UI, false);
	App->player1->Def_Target_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_TARGET, { 171, 55 }, { 85, 81 }, App->player1->Build_UI, false);
	App->player1->Mines_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_MINE, { 274, 55 }, { 85,81 }, App->player1->Build_UI, false);
	App->player1->Barracks_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_BARRACKS, { 375, 55 }, { 85, 81 }, App->player1->Build_UI, false);

	App->player1->Building_cost_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 110 , 27 }, { 0, 0 }, App->player1->Barracks_icon, false, { false, false });
	App->player1->Building_cost_text->label = App->player1->Building_cost_label;
	App->player1->Building_cost_text->color = {255, 0, 0, 255};

	// DEPLOY TROOPS UI ------------------

	App->player1->Deploy_UI = App->gui->AddUIElement(true, UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { 0,0 }, { 566, 163 }, nullptr, false);
	App->player1->Deploy_UI->rect = { 569, 411, 566, 163 };

	App->player1->Soldier_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_SOLDIER, { 68, 55 }, { 85, 81 }, App->player1->Deploy_UI, false);
	App->player1->Soldier_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 5 , 2 }, { 0, 0 }, App->player1->Soldier_icon, false, { false, false });
	App->player1->Soldier_text->label = App->player1->soldier_label;
	
	App->player1->Tankman_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_TANKMAN, { 171, 55 }, { 85, 81 }, App->player1->Deploy_UI, false);
	App->player1->Tankman_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 5 , 2 }, { 0, 0 }, App->player1->Tankman_icon, false, { false, false });
	App->player1->Tankman_text->label = App->player1->tankman_label;

	App->player1->Engineer_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_ENGINEER, { 274, 55 }, { 85, 81 }, App->player1->Deploy_UI, false);
	App->player1->Engineer_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 5 , 2 }, { 0, 0 }, App->player1->Engineer_icon, false, { false, false });
	App->player1->Engineer_text->label = App->player1->engineer_label;

	App->player1->Infiltrator_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_INFILTRATOR, { 377, 55 }, { 85, 81 }, App->player1->Deploy_UI, false);
	App->player1->Infiltrator_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 5 , 2 }, { 0, 0 }, App->player1->Infiltrator_icon, false, { false, false });
	App->player1->Infiltrator_text->label = App->player1->infiltrator_label;

	App->player1->War_hound_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_WARHOUND, { 480, 55 }, { 85, 81 }, App->player1->Deploy_UI, false);
	App->player1->War_hound_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 2 , 2 }, { 0, 0 }, App->player1->War_hound_icon, false, { false, false }); 
	App->player1->War_hound_text->label = App->player1->war_hound_label;

	App->player1->X_spawn = App->gui->AddUIElement(true, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 0 , 0 }, { 33,32 }, nullptr, false);
	App->player1->X_spawn->rect = {1443,357,33,32};
	App->player1->X_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 40 , 0 }, { 0, 0 }, App->player1->X_spawn, false, { false, false });
	App->player1->X_text->label = App->player1->X_label;
	App->player1->X_text->color = { 0,0,0,255 };

	App->player1->A_spawn = App->gui->AddUIElement(true, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 0 , 0 }, { 33,32 }, nullptr, false);
	App->player1->A_spawn->rect = { 1401,357,33,32 };
	App->player1->A_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 40 , 0 }, { 0, 0 }, App->player1->A_spawn, false, { false, false });
	App->player1->A_text->label = App->player1->A_label;
	App->player1->A_text->color = { 0,0,0,255 };

	
	//----- CAST -------------------------

	App->player1->Cast_UI = App->gui->AddUIElement(true, UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { 0,0 }, { 566, 163 }, nullptr, false);
	App->player1->Cast_UI->rect = { 0, 573, 566, 163 };

	App->player1->Cast2_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_CAST_TANKS, { 171 ,55 }, { 85, 81 }, App->player1->Cast_UI, false); 

	App->player1->Cast3_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_CAST_INVULNERABILITY, { 273, 55 }, { 85, 81 }, App->player1->Cast_UI, false);
	App->player1->invulnerable_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 35 , 0 }, { 0, 0 }, App->player1->Cast3_icon, false, { false, false });
	App->player1->invulnerable_text->label = App->player1->invulnerable_label;

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

	

	App->player1->LB_img = App->gui->AddUIElement(true, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { -5 , 70 }, { 55, 27 }, nullptr, false);
	App->player1->LB_img->rect = { 1269, 437, 55, 27 };
	App->player1->RB_img = App->gui->AddUIElement(true, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 585 , 70 }, { 55, 27 }, nullptr, false);
	App->player1->RB_img->rect = { 1334, 437, 55, 27 };

	//------ GENERAL BUILDINGS MENU --------

	App->player1->General_UI = App->gui->AddUIElement(true, UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { 0,0 }, { 566, 163 }, nullptr, false);
	App->player1->General_UI->rect = { 569, 575, 566, 163 };

	App->player1->Repair_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_REPAIR, { 70 , 55 }, { 62, 36 }, App->player1->General_UI, false);
	App->player1->Upgrade_icon = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_UPGRADE, { 70 ,105 }, { 62, 36 }, App->player1->General_UI, false);

	App->player1->RepairCost_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 90 , 0 }, { 0, 0 }, App->player1->Repair_icon, false, { false, false });
	App->player1->RepairCost_text->label = App->player1->repair_cost_label;
	App->player1->RepairCost_text->color = { 255, 0, 0, 255 };

	App->player1->UpgradeCost_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 90 , 0 }, { 0, 0 }, App->player1->Upgrade_icon, false, { false, false });
	App->player1->UpgradeCost_text->label = App->player1->upgrade_cost_label;
	App->player1->UpgradeCost_text->color = { 255, 0, 0, 255 };

	App->player1->Name_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 237 , 0 }, { 0, 0 }, App->player1->Repair_icon, false, { false, false });
	App->player1->Name_text->label = App->player1->name_label;
	App->player1->Name_text->color = { 255, 255, 255, 255 };

	App->player1->Health_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 237 , 30 }, { 0, 0 }, App->player1->Repair_icon, false, { false, false });
	App->player1->Health_text->label = App->player1->health_label;
	App->player1->Health_text->color = { 255, 255, 255, 255 };

	App->player1->Level_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 237 , 60 }, { 0, 0 }, App->player1->Repair_icon, false, { false, false });
	App->player1->Level_text->label = App->player1->level_label;
	App->player1->Level_text->color = { 255, 255, 255, 255 };

	App->player1->Create_troops = App->gui->AddUIElement(true, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 480 , 50 }, { 83, 35 }, nullptr, false);
	App->player1->Create_troops->rect = { 1194, 285, 83, 35 };

	App->player1->Create_abilities = App->gui->AddUIElement(true, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 480 , 110 }, { 83, 35 }, nullptr, false);
	App->player1->Create_abilities->rect = { 1194, 329, 84, 35 };

	//App->player1->Upgrade_troops = App->gui->AddUIElement(true, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 515 , -12 }, { 83, 35 }, nullptr, false);
	//App->player1->Upgrade_troops->rect = { 1280, 285, 83, 35 };


	//-------- CREATE TROOPS MENU ------------

	App->player1->Create_Troops_UI = App->gui->AddUIElement(true, UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { 0,0 }, { 566, 163 }, nullptr, false);
	App->player1->Create_Troops_UI->rect = { 0, 739, 566, 163 };

	App->player1->accept_button= App->gui->AddUIElement(true, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 470 , 60 }, { 80, 31 }, App->player1->Create_Troops_UI, false);
	App->player1->accept_button->rect = { 1277, 248, 80, 31 };

	App->player1->cancel_button = App->gui->AddUIElement(true, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 470 , 100 }, { 80, 31 }, App->player1->Create_Troops_UI, false);
	App->player1->cancel_button->rect = { 1194, 250, 81, 29 };

	App->player1->plus = App->gui->AddUIElement(true, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 390 , 50 }, { 51, 40 }, App->player1->Create_Troops_UI, false);
	App->player1->plus->rect = { 1444, 254, 51, 40 };

	App->player1->minus = App->gui->AddUIElement(true, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 270 , 50 }, { 51, 40 }, App->player1->Create_Troops_UI, false);
	App->player1->minus->rect = { 1379, 254, 52, 40 };

	App->player1->left_img = App->gui->AddUIElement(true, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 65 , 75 }, { 40, 40 }, App->player1->Create_Troops_UI, false);
	App->player1->left_img->rect = { 1397, 311, 40, 40 };

	App->player1->right_img = App->gui->AddUIElement(true, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 220 , 75 }, { 40, 40 }, App->player1->Create_Troops_UI, false);
	App->player1->right_img->rect = { 1437, 311, 40, 40 };

	App->player1->troop_icon = App->gui->AddUIElement(true, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 120 , 55 }, { 85, 81 }, App->player1->Create_Troops_UI, false);
	App->player1->troop_icon->rect = { 662, 0, 85, 81 };

	App->player1->num_troops_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 350, 65 }, { 0, 0 }, App->player1->Create_Troops_UI, false, { false, false });
	App->player1->num_troops_text->label = App->player1->num_troops_label;
	App->player1->num_troops_text->color = { 255, 255, 255,255 };

	App->player1->Troop_cost_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 320 , 110 }, { 0, 0 }, App->player1->Create_Troops_UI, false, { false, false });
	App->player1->Troop_cost_text->label = App->player1->Troop_cost_label;
	App->player1->Troop_cost_text->color = { 255, 0, 0, 255 };


	//---- GOLD ----- 
	App->player1->Gold_UI = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 470, 19 }, { 0, 0 }, nullptr, true, { false,false }, "$ 0000");
	App->player1->Gold_UI->color = { 255,255,0,255 };

	// ------ PAUSE MENU ------
	App->player1->Pause_UI = App->gui->AddUIElement(true, UI_Element::UI_type::TEXTURE, UI_Element::Action::NONE, { 0, 0 }, { App->win->width, App->win->height }, nullptr, false);
	App->player1->Pause_UI->texture = pause_soviet_texture;
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
	App->player1->Settings_UI->texture = pause_soviet_texture;
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
	App->player1->Abort_UI->texture = pause_soviet_texture;
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

	App->player1->win_screen = App->gui->AddUIElement(true, UI_Element::UI_type::TEXTURE, UI_Element::Action::NONE, { 0, 0 }, { App->win->width , App->win->height }, nullptr, false);
	App->player1->win_screen->texture = allied_win_tex;
	App->player1->win_screen->rect = { 0, 0, 0, App->win->height };

	App->player1->draw_screen = App->gui->AddUIElement(true, UI_Element::UI_type::TEXTURE, UI_Element::Action::NONE, { 0, 0 }, { App->win->width , App->win->height }, nullptr, false);
	App->player1->draw_screen->texture = draw_tex;
	App->player1->draw_screen->rect = { 0, 0, 0, App->win->height };

	//--- PLAYER 2
	//App->player2->Health_UI = App->gui->AddUIElement(false, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { x,y }, { w,h }, nullptr, true);
	//App->player2->Gold_UI = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { x,y }, { w,h }, nullptr, true, { false,false }, "$");

	

	App->player2->Main_UI = App->gui->AddUIElement(false, UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { App->win->width - 145 ,App->win->height + 122 }, { 566,163 }, nullptr, true);
	App->player2->Main_UI->rect = { 0, 408, 566, 163 };
	App->player2->Build_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_GOTO_BUILD, { 275,55 }, { 80, 81 }, App->player2->Main_UI, true);
	App->player2->Deploy_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_GOTO_DEPLOY, { 374,55 }, { 80, 81 }, App->player2->Main_UI, true);
	App->player2->Cast_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_GOTO_CAST, { 483,55 }, { 80,81 }, App->player2->Main_UI, true);

	App->player2->Y_to_Main = App->gui->AddUIElement(false, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { App->win->width - 140 , App->win->height + 192 }, { 39, 39 }, nullptr, false);
	App->player2->Y_to_Main->rect = { 1289, 373, 39, 39 }; 
	App->player2->Y_to_Main2 = App->gui->AddUIElement(false, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { App->win->width + 445 , App->win->height + 192 }, { 39, 39 }, nullptr, false);
	App->player2->Y_to_Main2->rect = { 1289, 373, 39, 39 };


	App->player2->SelectBuilding = App->gui->AddUIElement(false, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { App->win->width - 190 ,App->win->height + 152 }, { 39, 39 }, nullptr, true);
	App->player2->SelectBuilding->rect = { 1177, 488, 45, 107 };
	App->player2->In_SelectBuilding = App->gui->AddUIElement(false, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { App->win->width - 191 ,App->win->height + 152 }, { 39, 39 }, nullptr, false);
	App->player2->In_SelectBuilding->rect = { 1276, 488, 45, 107 };

	App->player2->Build_UI = App->gui->AddUIElement(false, UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { App->win->width - 145 ,App->win->height + 123 }, { 566, 163 }, nullptr, false);
	App->player2->Build_UI->rect = { 569, 246, 566, 163 };
	App->player2->Def_AOE_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_AOE, { 68, 55 }, { 85, 81 }, App->player2->Build_UI, false);
	App->player2->Def_Target_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_TARGET, { 171, 55 }, { 85, 81 }, App->player2->Build_UI, false);
	App->player2->Mines_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_MINE, { 274, 55 }, { 85,81 }, App->player2->Build_UI, false);
	App->player2->Barracks_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_BUILD_BARRACKS, { 375, 55 }, { 85, 81 }, App->player2->Build_UI, false);

	App->player2->Building_cost_text = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 110 , 27 }, { 0, 0 }, App->player2->Barracks_icon, false, { false, false });
	App->player2->Building_cost_text->label = App->player2->Building_cost_label;
	App->player2->Building_cost_text->color = { 255, 0, 0, 255 };

	// DEPLOY TROOPS UI ------------------

	App->player2->Deploy_UI = App->gui->AddUIElement(false, UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { App->win->width - 145 ,App->win->height + 123 }, { 566, 163 }, nullptr, false);
	App->player2->Deploy_UI->rect = { 569, 411, 566, 163 };

	App->player2->Soldier_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_SOLDIER, { 68, 55 }, { 85, 81 }, App->player2->Deploy_UI, false);
	App->player2->Soldier_text = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 5 , 2 }, { 0, 0 }, App->player2->Soldier_icon, false, { false, false });
	App->player2->Soldier_text->label = App->player2->soldier_label;

	App->player2->Tankman_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_TANKMAN, { 171, 55 }, { 85, 81 }, App->player2->Deploy_UI, false);
	App->player2->Tankman_text = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 5 , 2 }, { 0, 0 }, App->player2->Tankman_icon, false, { false, false });
	App->player2->Tankman_text->label = App->player2->tankman_label;

	App->player2->Engineer_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_ENGINEER, { 274, 55 }, { 85, 81 }, App->player2->Deploy_UI, false);
	App->player2->Engineer_text = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 5 , 2 }, { 0, 0 }, App->player2->Engineer_icon, false, { false, false });
	App->player2->Engineer_text->label = App->player2->engineer_label;

	App->player2->Infiltrator_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_INFILTRATOR, { 377, 55 }, { 85, 81 }, App->player2->Deploy_UI, false);
	App->player2->Infiltrator_text = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 5 , 2 }, { 0, 0 }, App->player2->Infiltrator_icon, false, { false, false });
	App->player2->Infiltrator_text->label = App->player2->infiltrator_label;

	App->player2->War_hound_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_DEPLOY_WARHOUND, { 480, 55 }, { 85, 81 }, App->player2->Deploy_UI, false);
	App->player2->War_hound_text = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 2 , 2 }, { 0, 0 }, App->player2->War_hound_icon, false, { false, false });
	App->player2->War_hound_text->label = App->player2->war_hound_label;

	App->player2->X_spawn = App->gui->AddUIElement(false, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 0 , 0 }, { 33,32 }, nullptr, false);
	App->player2->X_spawn->rect = { 1443,357,33,32 };
	App->player2->X_text = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 40 , 0 }, { 0, 0 }, App->player2->X_spawn, false, { false, false });
	App->player2->X_text->label = App->player2->X_label;
	App->player2->X_text->color = { 0,0,0,255 };

	App->player2->A_spawn = App->gui->AddUIElement(false, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 0 , 0 }, { 33,32 }, nullptr, false);
	App->player2->A_spawn->rect = { 1401,357,33,32 };
	App->player2->A_text = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 40 , 0 }, { 0, 0 }, App->player2->A_spawn, false, { false, false });
	App->player2->A_text->label = App->player2->A_label;
	App->player2->A_text->color = { 0,0,0,255 };

	//-----------------------------------------

	App->player2->Cast_UI = App->gui->AddUIElement(false, UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { App->win->width - 145 ,App->win->height + 123 }, { 566, 163 }, nullptr, false);
	App->player2->Cast_UI->rect = { 0, 573, 566, 163 };
	App->player2->Cast2_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_CAST_TANKS, { 171 ,55 }, { 85, 81 }, App->player2->Cast_UI, false);
	App->player2->Cast3_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_CAST_INVULNERABILITY, { 273, 55 }, { 85, 81 }, App->player2->Cast_UI, false);
	App->player2->invulnerable_text = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 35 , 0 }, { 0, 0 }, App->player2->Cast3_icon, false, { false, false });
	App->player2->invulnerable_text->label = App->player2->invulnerable_label;
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

	//------ GENERAL BUILDINGS MENU --------

	App->player2->General_UI = App->gui->AddUIElement(false, UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { App->win->width - 145 ,App->win->height + 123 }, { 566, 163 }, nullptr, false);
	App->player2->General_UI->rect = { 569, 575, 566, 163 };

	App->player2->Repair_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_REPAIR, { 70 ,55 }, { 62, 36 }, App->player2->General_UI, false);
	App->player2->Upgrade_icon = App->gui->AddUIElement(false, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::ACT_UPGRADE, { 70 ,105 }, { 62, 36 }, App->player2->General_UI, false);

	App->player2->RepairCost_text = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 90 , 0 }, { 0, 0 }, App->player2->Repair_icon, false, { false, false });
	App->player2->RepairCost_text->label = App->player2->repair_cost_label;
	App->player2->RepairCost_text->color = { 255, 0, 0, 255 };

	App->player2->UpgradeCost_text = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 90 , 0 }, { 0, 0 }, App->player2->Upgrade_icon, false, { false, false });
	App->player2->UpgradeCost_text->label = App->player2->upgrade_cost_label;
	App->player2->UpgradeCost_text->color = { 255, 0, 0, 255 };

	App->player2->Name_text = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 237 , 0 }, { 0, 0 }, App->player2->Repair_icon, false, { false, false });
	App->player2->Name_text->label = App->player2->name_label;
	App->player2->Name_text->color = { 255, 255, 255, 255 };

	App->player2->Health_text = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 237 , 30 }, { 0, 0 }, App->player2->Repair_icon, false, { false, false });
	App->player2->Health_text->label = App->player2->health_label;
	App->player2->Health_text->color = { 255, 255, 255, 255 };

	App->player2->Level_text = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 237 , 60 }, { 0, 0 }, App->player2->Repair_icon, false, { false, false });
	App->player2->Level_text->label = App->player2->level_label;
	App->player2->Level_text->color = { 255, 255, 255, 255 };

	App->player2->Create_troops = App->gui->AddUIElement(false, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { App->win->width + 330 , App->win->height + 175 }, { 83, 35 }, nullptr, false);
	App->player2->Create_troops->rect = { 1194, 285, 83, 35 };

	App->player2->Create_abilities = App->gui->AddUIElement(false, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { App->win->width + 330 , App->win->height + 233 }, { 83, 35 }, nullptr, false);
	App->player2->Create_abilities->rect = { 1194, 329, 84, 35 };

	//-------- CREATE TROOPS MENU ------------

	App->player2->Create_Troops_UI = App->gui->AddUIElement(false, UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { App->win->width - 145 ,App->win->height + 123 }, { 566, 163 }, nullptr, false);
	App->player2->Create_Troops_UI->rect = { 0, 739, 566, 163 };

	App->player2->accept_button = App->gui->AddUIElement(false, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 470 , 60 }, { 80, 31 }, App->player2->Create_Troops_UI, false);
	App->player2->accept_button->rect = { 1277, 248, 80, 31 };

	App->player2->cancel_button = App->gui->AddUIElement(false, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 470 , 100 }, { 80, 31 }, App->player2->Create_Troops_UI, false);
	App->player2->cancel_button->rect = { 1194, 250, 81, 29 };

	App->player2->plus = App->gui->AddUIElement(false, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 390 , 50 }, { 51, 40 }, App->player2->Create_Troops_UI, false);
	App->player2->plus->rect = { 1444, 254, 51, 40 };

	App->player2->minus = App->gui->AddUIElement(false, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 270 , 50 }, { 51, 40 }, App->player2->Create_Troops_UI, false);
	App->player2->minus->rect = { 1379, 254, 52, 40 };

	App->player2->left_img = App->gui->AddUIElement(false, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 65 , 75 }, { 40, 40 }, App->player2->Create_Troops_UI, false);
	App->player2->left_img->rect = { 1397, 311, 40, 40 };

	App->player2->right_img = App->gui->AddUIElement(false, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 220 , 75 }, { 40, 40 }, App->player2->Create_Troops_UI, false);
	App->player2->right_img->rect = { 1437, 311, 40, 40 };

	App->player2->troop_icon = App->gui->AddUIElement(false, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 120 , 55 }, { 85, 81 }, App->player2->Create_Troops_UI, false);
	App->player2->troop_icon->rect = { 662, 0, 85, 81 };

	App->player2->num_troops_text = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 350, 65 }, { 0, 0 }, App->player2->Create_Troops_UI, false, { false, false });
	App->player2->num_troops_text->label = App->player2->num_troops_label;
	App->player2->num_troops_text->color = { 255, 255, 255,255 };

	App->player2->Troop_cost_text = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 320 , 110 }, { 0, 0 }, App->player2->Create_Troops_UI, false, { false, false });
	App->player2->Troop_cost_text->label = App->player2->Troop_cost_label;
	App->player2->Troop_cost_text->color = { 255, 0, 0, 255 };

	// ------- GOLD

	App->player2->Gold_UI = App->gui->AddUIElement(false, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 2000, 1191 }, { 0, 0 }, nullptr, true, { false,false }, "$ 0000");
	App->player2->Gold_UI->color = { 255,255,0,255 };

	//------- PAUSE MENU ------

	App->player2->Pause_UI = App->gui->AddUIElement(false, UI_Element::UI_type::TEXTURE, UI_Element::Action::NONE, { 0, 0 }, { App->win->width, App->win->height }, nullptr, false);
	App->player2->Pause_UI->texture = pause_alied_texture;
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

	App->player2->LB_img = App->gui->AddUIElement(false, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { App->win->width - 150 , App->win->height + 192 }, { 55, 27 }, nullptr, false);
	App->player2->LB_img->rect = { 1269, 437, 55, 27 };
	App->player2->RB_img = App->gui->AddUIElement(false, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { App->win->width + 440 , App->win->height + 192 }, { 55, 27 }, nullptr, false);
	App->player2->RB_img->rect = { 1334, 437, 55, 27 };

	

	

	//------ Settings Pause MENU ------
	App->player2->Settings_UI = App->gui->AddUIElement(false, UI_Element::UI_type::TEXTURE, UI_Element::Action::NONE, { 0, 0 }, { App->win->width - 400, App->win->height }, nullptr, false);
	App->player2->Settings_UI->texture = pause_alied_texture;
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
	App->player2->Abort_UI->texture = pause_alied_texture;
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

	App->player2->win_screen  = App->gui->AddUIElement(false, UI_Element::UI_type::TEXTURE, UI_Element::Action::NONE, { 0, 0 }, { App->win->width , App->win->height }, nullptr, false);
	App->player2->win_screen->texture = soviet_win_tex;
	App->player2->win_screen->rect = { 0, 0, 0, App->win->height };

	App->player2->draw_screen = App->gui->AddUIElement(false, UI_Element::UI_type::TEXTURE, UI_Element::Action::NONE, { 0, 0 }, { App->win->width , App->win->height }, nullptr, false);
	App->player2->draw_screen->texture = draw_tex;
	App->player2->draw_screen->rect = { 0, 0, 0, App->win->height };


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

	//pair <int, int> test;
	//Collider collider_test;
	//test.first = 0;
	//test.second = 0;
	//App->entitymanager->AddEntity(true, Entity::entityType::WALLS, test, collider_test);
	//App->entitymanager->AddEntity(false, Entity::entityType::WALLS, test, collider_test);
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


	App->player1->UpdateFocus(App->player1->currentUI);
	App->player2->UpdateFocus(App->player2->currentUI);
	

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
				if (App->player1->gold_added == false)
				{
					App->player1->gold_persecond += (*item)->production;
				}
							
			}
		}
		App->player1->gold_added = true;

		if ((int)world_clock.ReadSec() > App->player1->time_iterator)
		{
			App->player1->gold += App->player1->gold_persecond;
			App->player1->gold_persecond = 0;
			App->player1->gold_added = false;
			App->player1->time_iterator = (int)world_clock.ReadSec();
		}
		

		// Gold Player 2 -----------
		item = App->player2->buildings.begin();
		for (item; item != App->player2->buildings.end(); ++item)
		{
			if ((*item)->type == Entity::entityType::TOWNHALL || (*item)->type == Entity::entityType::MINES)
			{
				if (App->player2->gold_added == false)
				{
					App->player2->gold_persecond += (*item)->production;
				}

			}
		}
		App->player2->gold_added = true;

		if ((int)world_clock.ReadSec() > App->player2->time_iterator)
		{
			App->player2->gold += App->player2->gold_persecond;
			App->player2->gold_persecond = 0;
			App->player2->gold_added = false;
			App->player2->time_iterator = (int)world_clock.ReadSec();
		}
		
	}
	
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	BROFILER_CATEGORY("Scene Update", Profiler::Color::DarkOrange);

	pair<int, int> pos;
	App->input->GetMousePosition(pos.first, pos.second);
	pos = App->render->ScreenToWorld(pos.first, pos.second);
	pos.first--;

	LOG("UI: %u", App->player1->currentUI);

	//Enter GodMode
	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) //Godmode
	{
		godmode = !godmode;
	}
	//else if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN) //Framerate Cap On/Off
	//{
	//	App->fpsCapON = !App->fpsCapON;
	//}

	//Debug functionalities that can be used anywhere
	if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN) //Debug UI
	{
		App->gui->UI_Debug = !App->gui->UI_Debug;
	}
	
	if (App->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN)
	{
		App->map->debug = !App->map->debug;
		App->player1->isBuilding = !App->player1->isBuilding;
	}
	if (App->input->GetKey(SDL_SCANCODE_F12) == KEY_DOWN)
	{
		App->map->debug = !App->map->debug;
		App->player2->isBuilding = !App->player2->isBuilding;
	}

	//Debug functionalities in-game. Press F10 to enter GodMode and activate said features
	if (App->scene->active && !App->scene->pause && godmode)
	{
		
		debug_text->visible = true;
		//Keyboard debug
		// Player 1 -> number / Player2 -> letter
		/*if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		{
			App->entitymanager->AddEntity(true, Entity::entityType::TOWNHALL, {pos.first,pos.second},App->player1->collider);
		}
		else if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN)
		{
			App->entitymanager->AddEntity(false, Entity::entityType::TOWNHALL, {pos.first,pos.second},App->player2->collider);
		}*/
		if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
		{
			App->entitymanager->AddEntity(true, Entity::entityType::MAIN_DEFENSE, {pos.first,pos.second},App->player1->collider);
		}
		else if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
		{
			App->entitymanager->AddEntity(false, Entity::entityType::MAIN_DEFENSE, {pos.first,pos.second},App->player2->collider);
		}
		else if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
		{
			App->entitymanager->AddEntity(true, Entity::entityType::COMMAND_CENTER, {pos.first,pos.second},App->player1->collider);
		}
		else if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
		{
			App->entitymanager->AddEntity(false, Entity::entityType::COMMAND_CENTER, {pos.first,pos.second},App->player2->collider);
		}
		else if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
		{
			App->entitymanager->AddEntity(true, Entity::entityType::BARRACKS, {pos.first,pos.second},App->player1->collider);
		}
		else if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
		{
			App->entitymanager->AddEntity(false, Entity::entityType::BARRACKS, {pos.first,pos.second},App->player2->collider);
		}
		else if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN)
		{
			App->entitymanager->AddEntity(true, Entity::entityType::DEFENSE_AOE, {pos.first,pos.second},App->player1->collider);
		}
		else if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN)
		{
			App->entitymanager->AddEntity(false, Entity::entityType::DEFENSE_AOE, {pos.first,pos.second},App->player2->collider);
		}
		else if (App->input->GetKey(SDL_SCANCODE_6) == KEY_DOWN)
		{
			App->entitymanager->AddEntity(true, Entity::entityType::DEFENSE_TARGET, {pos.first,pos.second},App->player1->collider);
		}
		else if (App->input->GetKey(SDL_SCANCODE_Y) == KEY_DOWN)
		{
			App->entitymanager->AddEntity(false, Entity::entityType::DEFENSE_TARGET, {pos.first,pos.second},App->player2->collider);
		}
		else if (App->input->GetKey(SDL_SCANCODE_7) == KEY_DOWN)
		{
			App->entitymanager->AddEntity(true, Entity::entityType::MINES, {pos.first,pos.second},App->player1->collider);
		}
		else if (App->input->GetKey(SDL_SCANCODE_U) == KEY_DOWN)
		{
			App->entitymanager->AddEntity(false, Entity::entityType::MINES, {pos.first,pos.second},App->player2->collider);
		}
		else if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		{
			App->entitymanager->AddEntity(true, Entity::entityType::SOLDIER, {pos.first,pos.second},App->player1->collider);
		}
		else if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN)
		{
			App->entitymanager->AddEntity(false, Entity::entityType::SOLDIER, {pos.first,pos.second},App->player2->collider);
		}
		else if (App->input->GetKey(SDL_SCANCODE_0) == KEY_DOWN)
		{
			App->entitymanager->AddEntity(true, Entity::entityType::WALLS, {pos.first,pos.second},App->player1->collider);
		}
		else if (App->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN)
		{
			App->entitymanager->AddEntity(false, Entity::entityType::WALLS, {pos.first,pos.second},App->player2->collider);
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
		else if (App->input->GetKey(SDL_SCANCODE_I) == KEY_REPEAT) //View colliders
		{
			App->render->zoom += 0.01f;
		}
		else if (App->input->GetKey(SDL_SCANCODE_K) == KEY_REPEAT) //View colliders
		{
			App->render->zoom -= 0.01f;
		}
		else if (App->input->GetKey(SDL_SCANCODE_X) == KEY_REPEAT) //View colliders
		{
			worldminutes = 10;
		}
    
		//Timer debug
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
		{
			if (pausetimer == false)
			{
				//if the pause timer is false, the clock is running and you want to stop it
				pausetimer = true;
				world_seconds.Stop();
			}
			else if (pausetimer == true)
			{
				//if the pause timer is true, the clock is stop and you want to start it
				pausetimer = false;
				world_seconds.Start();
			}
		}

		
		//Mouse debug
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

				pair<int, int> final;
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
	}

	if (!godmode)
		debug_text->visible = false;

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

	App->map->DrawWalkability(dt);
	App->entitymanager->Draw(dt);
	App->map->DrawDecorations(dt);
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
		else if ((*item) == App->player1->num_troops_text) //CREATING TROOPS: number of troops
		{
			sprintf_s(App->player1->num_troops_label, "%i", App->player1->number_of_troops);
			(*item)->label = App->player1->num_troops_label;
		}
		else if ((*item) == App->player1->Soldier_text) //NUMBER OF SOLDIERS
		{
			sprintf_s(App->player1->soldier_label, "%i", App->player1->SoldiersCreated);
			

			if (App->player1->SoldiersCreated > 0)
			{
				(*item)->color = {0, 0, 0 , 255}; //BLACK
			}
			else
			{
				(*item)->color = { 255 , 0, 0 , 255 }; //RED
			}
		}
		else if ((*item) == App->player1->Tankman_text) //NUMBER OF TANKMANS
		{
			sprintf_s(App->player1->tankman_label, "%i", App->player1->TankmansCreated);
			

			if (App->player1->TankmansCreated > 0)
			{
				(*item)->color = { 0, 0, 0 , 255 }; //BLACK
			}
			else
			{
				(*item)->color = { 255 , 0, 0 , 255 }; //RED
			}
		}
		else if ((*item) == App->player1->Infiltrator_text) //NUMBER OF INFILTRATORS
		{
			sprintf_s(App->player1->infiltrator_label, "%i", App->player1->InfiltratorsCreated);


			if (App->player1->InfiltratorsCreated > 0)
			{
				(*item)->color = { 0, 0, 0 , 255 }; //BLACK
			}
			else
			{
				(*item)->color = { 255 , 0, 0 , 255 }; //RED
			}
		}
		else if ((*item) == App->player1->Engineer_text) //NUMBER OF ENGINEERS
		{
			sprintf_s(App->player1->engineer_label, "%i", App->player1->EngineersCreated);


			if (App->player1->EngineersCreated > 0)
			{
				(*item)->color = { 0, 0, 0 , 255 }; //BLACK
			}
			else
			{
				(*item)->color = { 255 , 0, 0 , 255 }; //RED
			}
		}
		else if ((*item) == App->player1->War_hound_text) //NUMBER OF WAR_HOUNDS
		{
			sprintf_s(App->player1->war_hound_label, "%i", App->player1->WarHoundsCreated);


			if (App->player1->WarHoundsCreated > 0)
			{
				(*item)->color = { 0, 0, 0 , 255 }; //BLACK
			}
			else
			{
				(*item)->color = { 255 , 0, 0 , 255 }; //RED
			}
		}
		else if ((*item) == App->player1->invulnerable_text) //NUMBER OF INVULNERABLE ABILITIES
		{
			sprintf_s(App->player1->invulnerable_label, "%i", App->player1->Invulnerable_abilities);


			if (App->player1->Invulnerable_abilities > 0)
			{
				(*item)->color = { 0, 255, 0 , 255 }; //BLACK
			}
			else
			{
				(*item)->color = { 255 , 0, 0 , 255 }; //RED
			}
			
		}
		else if ((*item) == App->player1->Building_cost_text) //BUILDING COST
		{
			
			sprintf_s(App->player1->Building_cost_label, "%i $", App->player1->BuildingCost);
	
		}
		else if ((*item) == App->player1->Troop_cost_text) //TROOP COST
		{

			sprintf_s(App->player1->Troop_cost_label, "%i $", App->player1->TroopCost);

		}
		else if ((*item) == App->player1->Music_Slider_Button) //MUSIC SLIDER POS
		{
			App->player1->Music_Slider_Button->position.first = (550 * App->audio->musicVolume) / 100;
		}
		else if ((*item) == App->player1->FX_Slider_Button) //FX SLIDER POS
		{
			App->player1->FX_Slider_Button->position.first = (550 * App->audio->sfxVolume) / 100;
		}
		else if ((*item) == App->player1->X_spawn && App->player1->X_spawn->visible == true)
		{
			pair <int, int> pos = App->map->MapToWorld(App->player1->currentTile.first, App->player1->currentTile.second);
			App->player1->X_spawn->globalpos = App->render->WorldtoScreen(pos.first, pos.second);
			App->player1->X_spawn->globalpos.first += 200;
			App->player1->X_spawn->globalpos.second -= 70;
			
		}
		else if ((*item) == App->player1->A_spawn && App->player1->A_spawn->visible == true)
		{
		pair <int, int> pos = App->map->MapToWorld(App->player1->currentTile.first, App->player1->currentTile.second);
		App->player1->A_spawn->globalpos = App->render->WorldtoScreen(pos.first, pos.second);
		App->player1->A_spawn->globalpos.first += 200;
		App->player1->A_spawn->globalpos.second -= 40;

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
		else if ((*item) == App->player2->num_troops_text) //CREATING TROOPS: number of troops
		{
			sprintf_s(App->player2->num_troops_label, "%i", App->player2->number_of_troops);
			(*item)->label = App->player2->num_troops_label;
		}
		else if ((*item) == App->player2->Soldier_text) //NUMBER OF SOLDIERS
		{
			sprintf_s(App->player2->soldier_label, "%i", App->player2->SoldiersCreated);


			if (App->player2->SoldiersCreated > 0)
			{
				(*item)->color = { 0, 0, 0 , 255 }; //BLACK
			}
			else
			{
				(*item)->color = { 255 , 0, 0 , 255 }; //RED
			}
		}
		else if ((*item) == App->player2->Tankman_text) //NUMBER OF TANKMANS
		{
			sprintf_s(App->player2->tankman_label, "%i", App->player2->TankmansCreated);


			if (App->player2->TankmansCreated > 0)
			{
				(*item)->color = { 0, 0, 0 , 255 }; //BLACK
			}
			else
			{
				(*item)->color = { 255 , 0, 0 , 255 }; //RED
			}
		}
		else if ((*item) == App->player2->Infiltrator_text) //NUMBER OF INFILTRATORS
		{
			sprintf_s(App->player2->infiltrator_label, "%i", App->player2->InfiltratorsCreated);


			if (App->player2->InfiltratorsCreated > 0)
			{
				(*item)->color = { 0, 0, 0 , 255 }; //BLACK
			}
			else
			{
				(*item)->color = { 255 , 0, 0 , 255 }; //RED
			}
		}
		else if ((*item) == App->player2->Engineer_text) //NUMBER OF ENGINEERS
		{
			sprintf_s(App->player2->engineer_label, "%i", App->player2->EngineersCreated);


			if (App->player2->EngineersCreated > 0)
			{
				(*item)->color = { 0, 0, 0 , 255 }; //BLACK
			}
			else
			{
				(*item)->color = { 255 , 0, 0 , 255 }; //RED
			}
		}
		else if ((*item) == App->player2->War_hound_text) //NUMBER OF WAR_HOUNDS
		{
			sprintf_s(App->player2->war_hound_label, "%i", App->player2->WarHoundsCreated);


			if (App->player2->WarHoundsCreated > 0)
			{
				(*item)->color = { 0, 0, 0 , 255 }; //BLACK
			}
			else
			{
				(*item)->color = { 255 , 0, 0 , 255 }; //RED
			}
		}
		else if ((*item) == App->player2->invulnerable_text) //NUMBER OF INVULNERABLE ABILITIES
		{
			sprintf_s(App->player2->invulnerable_label, "%i", App->player2->Invulnerable_abilities);


			if (App->player2->Invulnerable_abilities > 0)
			{
				(*item)->color = { 0, 255, 0 , 255 }; //GREEN
			}
			else
			{
				(*item)->color = { 255 , 0, 0 , 255 }; //RED
			}
		}
		else if ((*item) == App->player2->Building_cost_text) //BUILDING COST
		{

			sprintf_s(App->player2->Building_cost_label, "%i $", App->player2->BuildingCost);

		}
		else if ((*item) == App->player2->Troop_cost_text) //TROOP COST
		{

			sprintf_s(App->player2->Troop_cost_label, "%i $", App->player2->TroopCost);

		}
		else if ((*item) == App->player2->Music_Slider_Button) //MUSIC SLIDER POS
		{
			App->player2->Music_Slider_Button->position.first = (550 * App->audio->musicVolume) / 100;
		}
		else if ((*item) == App->player2->FX_Slider_Button) //FX SLIDER POS
		{
			App->player2->FX_Slider_Button->position.first = (550 * App->audio->sfxVolume) / 100;
		}
		else if ((*item) == App->player2->X_spawn && App->player2->X_spawn->visible == true)
		{
			pair <int, int> pos = App->map->MapToWorld(App->player2->currentTile.first, App->player2->currentTile.second);
			App->player2->X_spawn->globalpos = App->render->WorldtoScreen(pos.first, pos.second);
			App->player2->X_spawn->globalpos.first += 200;
			App->player2->X_spawn->globalpos.second -= 70;

		}
		else if ((*item) == App->player2->A_spawn && App->player2->A_spawn->visible == true)
		{
			pair <int, int> pos = App->map->MapToWorld(App->player2->currentTile.first, App->player2->currentTile.second);
			App->player2->A_spawn->globalpos = App->render->WorldtoScreen(pos.first, pos.second);
			App->player2->A_spawn->globalpos.first += 200;
			App->player2->A_spawn->globalpos.second -= 40;

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
	App->tex->UnLoad(allied_win_tex);
	App->tex->UnLoad(soviet_win_tex);
	App->tex->UnLoad(pause_soviet_texture);
	App->tex->UnLoad(pause_alied_texture);
	

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
	//SpawnEntities();

	return true;
}

void Scene::SpawnEntities()
{


	//--- PLAYER 1
	pair<int, int> map_pos = App->map->WorldToMap(App->map->data.main_building2.first , App->map->data.main_building2.second);
	App->player1->Townhall = App->entitymanager->AddEntity(true, Entity::entityType::TOWNHALL, App->map->data.main_building2, App->player1->GetCollider({ 4,4 }, { map_pos.first, map_pos.second  }));

	
	map_pos = App->map->WorldToMap(App->map->data.special_skill.first, App->map->data.special_skill.second);
	App->entitymanager->AddEntity(true, Entity::entityType::COMMAND_CENTER, App->map->data.special_skill, App->player1->GetCollider({ 4,3 }, { map_pos.first , map_pos.second }));

	map_pos = App->map->WorldToMap(App->map->data.main_tower.first, App->map->data.main_tower.second);

	App->entitymanager->AddEntity(true, Entity::entityType::DEFENSE_TARGET, App->map->data.main_tower, App->player1->GetCollider({ 2,2 }, { map_pos.first , map_pos.second }));

	/*map_pos = App->map->WorldToMap(App->map->data.barrack.first, App->map->data.barrack.second);
	App->entitymanager->AddEntity(true, Entity::entityType::BARRACKS, App->map->data.barrack, App->player1->GetCollider({ 3,4 }, { map_pos.first , map_pos.second }));

	map_pos = App->map->WorldToMap(App->map->data.tower.first, App->map->data.tower.second);
	App->entitymanager->AddEntity(true, Entity::entityType::MAIN_DEFENSE, App->map->data.tower, App->player1->GetCollider({ 2,2 }, { map_pos.first , map_pos.second }));

	map_pos = App->map->WorldToMap(App->map->data.tower2.first, App->map->data.tower2.second);
	App->entitymanager->AddEntity(true, Entity::entityType::MINES, App->map->data.tower2, App->player1->GetCollider({ 4,4 }, { map_pos.first , map_pos.second }));
*/


	//--- PLAYER 2
	map_pos = App->map->WorldToMap(App->map->data.main_building.first, App->map->data.main_building.second);
	App->player2->Townhall = App->entitymanager->AddEntity(false, Entity::entityType::TOWNHALL, App->map->data.main_building, App->player2->GetCollider({ 7,3 }, { map_pos.first , map_pos.second }));
	
	//--- WALLS
	LoadWalls();

	map_pos = App->map->WorldToMap(App->map->data.special_skill2.first, App->map->data.special_skill2.second);
	App->entitymanager->AddEntity(false, Entity::entityType::COMMAND_CENTER, App->map->data.special_skill2, App->player2->GetCollider({ 4,3 }, { map_pos.first, map_pos.second}));

	map_pos = App->map->WorldToMap(App->map->data.main_tower2.first, App->map->data.main_tower2.second);
	App->entitymanager->AddEntity(false, Entity::entityType::DEFENSE_TARGET, App->map->data.main_tower2, App->player2->GetCollider({ 2,2 }, { map_pos.first , map_pos.second}));


	//App->entitymanager->AddEntity(false, Entity::entityType::MAIN_DEFENSE, { 50,50 });
	//App->entitymanager->AddEntity(false, Entity::entityType::WALLS, { 50,50 });
	//App->entitymanager->AddEntity(false, Entity::entityType::SOLDIER, { 5000,50 });
	//App->entitymanager->AddEntity(false, Entity::entityType::SOLDIER, { 350,400 });


}

//void Scene::changeSize(float time, int maxsize)
//{
//	Timer time
//}


void Scene::LoadWalls()
{
	// Load Animations
	Animation* current_anim = nullptr;
	Animation* front = nullptr;
	Animation* side = nullptr;
	Animation* corner_down_left = nullptr;
	Animation* tower = nullptr;
	Animation* corner_down_right = nullptr;
	Animation* corner_up_left = nullptr;
	Animation* corner_up_right = nullptr;

	front = front->LoadAnimation("animation/walls.tmx", "principal_wall");
	side = side->LoadAnimation("animation/walls.tmx", "side_wall");
	corner_down_left = corner_down_left->LoadAnimation("animation/walls.tmx", "corner_east");
	tower = tower->LoadAnimation("animation/walls.tmx", "small_wall");
	corner_down_right = corner_down_right->LoadAnimation("animation/walls.tmx", "corner_north");
	corner_up_left = corner_up_left->LoadAnimation("animation/walls.tmx", "corner_south");
	corner_up_right = corner_up_right->LoadAnimation("animation/walls.tmx", "corner_west");

	//---
	Entity* entity;
	Collider collider;

	pair<int, int> pos;


	//--- Player 2
	list<pair<int, int>>::iterator item = App->map->data.wall_list.begin();
	while (item != App->map->data.wall_list.end())
	{
		pos = App->map->MapToWorld((*item).first, (*item).second);
		collider = App->player2->GetCollider({ 1,1 }, {(*item).first, (*item).second});
		bool right = false, left = false, up = false, down = false;

		list<pair<int, int>>::iterator item2 = App->map->data.wall_list.begin();
		while (item2 != App->map->data.wall_list.end())
		{
			if ((*item2).first - 1 == (*item).first && (*item2).second == (*item).second)
			{
				right = true;
			}
			else if ((*item2).first + 1 == (*item).first && (*item2).second == (*item).second)
			{																			
				left = true;															
			}																			
			else if ((*item2).first == (*item).first && (*item2).second - 1 == (*item).second)
			{													
				down = true;									
			}													
			else if ((*item2).first == (*item).first && (*item2).second + 1 == (*item).second)
			{
				up = true;
			}

			item2++;
		}

		if (down && up)
		{
			//side
			current_anim = side;
		}
		else if (left && right)
		{
			//frontal
			current_anim = front;
		}
		else if (down && right)
		{
			//corner
			current_anim = corner_down_right;
		}
		else if (down && left)
		{
			//corner
			current_anim = corner_down_left;
		}
		else if (up && left)
		{
			//corner
			current_anim = corner_up_left;
		}
		else if (up && right)
		{
			//corner
			current_anim = corner_up_right;
		}
		else if (up || down || left || right)
		{
			//only tower
			current_anim = tower;
		}

		App->entitymanager->AddEntity(false, Entity::entityType::WALLS, *item, collider, current_anim);
		//entity = new Walls(false, *item, collider, current_anim); //add entity
		//App->player2->buildings.push_back((Building*)entity);
		//App->entitymanager->entity_list.push_back(entity);

		//App->player1->UpdateWalkabilityMap(false, collider);

		item++;
	}

	//--- Player 1
	item = App->map->data.wall_list2.begin();
	while (item != App->map->data.wall_list2.end())
	{
		pos = App->map->MapToWorld((*item).first, (*item).second);
		collider = App->player1->GetCollider({ 1,1 }, { (*item).first , (*item).second});
		bool right = false, left = false, up = false, down = false;

		list<pair<int, int>>::iterator item2 = App->map->data.wall_list2.begin();
		while (item2 != App->map->data.wall_list2.end())
		{
			if ((*item2).first - 1 == (*item).first && (*item2).second == (*item).second)
			{
				right = true;
			}
			else if ((*item2).first + 1 == (*item).first && (*item2).second == (*item).second)
			{
				left = true;
			}
			else if ((*item2).first == (*item).first && (*item2).second - 1 == (*item).second)
			{
				down = true;
			}
			else if ((*item2).first == (*item).first && (*item2).second + 1 == (*item).second)
			{
				up = true;
			}

			item2++;
		}

		if (down && up)
		{
			//side
			current_anim = side;
		}
		else if (left && right)
		{
			//frontal
			current_anim = front;
		}
		else if (down && right)
		{
			//corner
			current_anim = corner_down_right;
		}
		else if (down && left)
		{
			//corner
			current_anim = corner_down_left;
		}
		else if (up && left)
		{
			//corner
			current_anim = corner_up_left;
		}
		else if (up && right)
		{
			//corner
			current_anim = corner_up_right;
		}
		else if (up || down || left || right)
		{
			//only tower
			current_anim = tower;
		}

		entity = new Walls(true, *item, collider, current_anim); //add entity
		App->player1->buildings.push_back((Building*)entity);
		App->entitymanager->entity_list.push_back(entity);
		App->player2->UpdateWalkabilityMap(false, collider);

		item++;
	}
}

void Scene::DrawLiveBar(Player* player)
{
	player->LiveBar.w = (348 * player->health) / player->max_health; // (maximum rect width * town hall live) / MAX town hall live

	if (player->health >= player->max_health - (player->max_health / 3))
		App->render->DrawQuad(player->LiveBar, 0, 255, 0, 255, true, false); //green

	else if (player->health >= player->max_health / 4 && player->health <= player->max_health - (player->max_health / 3))
		App->render->DrawQuad(player->LiveBar, 255, 150, 0, 255, true, false); //orange

	else
		App->render->DrawQuad(player->LiveBar, 255, 0, 0, 255, true, false); //red
}

void Scene::Victorious(Player* player, float dt)
{
	pair<int, int> tmp_pos1 = App->player1->Townhall->GetPos();
	pair<int, int> tmp_pos2 = App->player2->Townhall->GetPos();
	App->map->explosion_anim->speed = 1.2f;
	//Explosion, after it has finished, blit continue with function
	if (player == App->player1 && !App->map->explosion_anim->Finished())
	{
		App->audio->PlayFx(FINAL_EXPLOSION);
		App->render->Blit(App->scene->explosion_tex, tmp_pos1.first, tmp_pos1.second, &App->map->explosion_anim->GetCurrentFrame(dt));
		App->render->Blit(App->scene->explosion_tex, tmp_pos1.first + 58, tmp_pos1.second + 42, &App->map->explosion_anim->GetCurrentFrame(dt));
		App->render->Blit(App->scene->explosion_tex, tmp_pos1.first - 73, tmp_pos1.second + 86, &App->map->explosion_anim->GetCurrentFrame(dt));
		App->render->Blit(App->scene->explosion_tex, tmp_pos1.first + 37, tmp_pos1.second - 76, &App->map->explosion_anim->GetCurrentFrame(dt));
		App->render->Blit(App->scene->explosion_tex, tmp_pos1.first - 42, tmp_pos1.second - 19, &App->map->explosion_anim->GetCurrentFrame(dt));
	}
	else if (player == App->player2 && !App->map->explosion_anim->Finished())
	{
		App->audio->PlayFx(FINAL_EXPLOSION);
		App->render->Blit(App->scene->explosion_tex, tmp_pos2.first, tmp_pos2.second, &App->map->explosion_anim->GetCurrentFrame(dt));
		App->render->Blit(App->scene->explosion_tex, tmp_pos2.first + 8, tmp_pos2.second + 22, &App->map->explosion_anim->GetCurrentFrame(dt));
		App->render->Blit(App->scene->explosion_tex, tmp_pos2.first - 3, tmp_pos2.second + 16, &App->map->explosion_anim->GetCurrentFrame(dt));
		App->render->Blit(App->scene->explosion_tex, tmp_pos2.first + 17, tmp_pos2.second - 6, &App->map->explosion_anim->GetCurrentFrame(dt));
		App->render->Blit(App->scene->explosion_tex, tmp_pos2.first - 12, tmp_pos2.second - 9, &App->map->explosion_anim->GetCurrentFrame(dt));
		App->render->Blit(App->scene->explosion_tex, tmp_pos2.first + 120, tmp_pos2.second + 18, &App->map->explosion_anim->GetCurrentFrame(dt));
		App->render->Blit(App->scene->explosion_tex, tmp_pos2.first - 120, tmp_pos2.second - 18, &App->map->explosion_anim->GetCurrentFrame(dt));
	}

	if (player == App->player1 && App->map->explosion_anim->Finished())
	{
		pausetimer = true;
		world_seconds.Stop();

		App->player2->currentUI = Player::CURRENT_UI::ENDGAME;
		App->player2->UpdateVisibility();
		App->player1->currentUI = Player::CURRENT_UI::CURR_WIN_SCREEN;
		App->player1->UpdateVisibility();

		endgame = true;

		if (App->scene->pause == false)
		{
			App->scene->pause = true;
		}
	}
	else if (player == App->player2 && App->map->explosion_anim->Finished())
	{
		pausetimer = true;
		world_seconds.Stop();

		App->player1->currentUI = Player::CURRENT_UI::ENDGAME;
		App->player1->UpdateVisibility();
		App->player2->currentUI = Player::CURRENT_UI::CURR_WIN_SCREEN;
		App->player2->UpdateVisibility();

		endgame = true;

		if (App->scene->pause == false)
		{
			App->scene->pause = true;
		}
	}

}
void Scene::MatchDraw()
{
	pausetimer = true;
	world_seconds.Stop();

	App->player1->currentUI = Player::CURRENT_UI::DRAW;
	App->player1->UpdateVisibility();
	App->player2->currentUI = Player::CURRENT_UI::ENDGAME;
	App->player2->UpdateVisibility();

	endgame = true;

	if (App->scene->pause == false)
	{
		App->scene->pause = true;
	}
}

void Scene::ResetGame()
{
	App->scene->pause = false;

	pausetimer = false;
	world_seconds.Start();

	
	App->entitymanager->Disable();
	App->player2->Disable();
	App->player1->Disable();
	App->gui->Disable();
	App->map->Disable();
	App->font->Disable();

	App->scenechange->ChangeMap(-1, 2);
	App->scene->Disable(); //Here fadetoblack
	App->scene->Enable();

	App->font->Enable();
	App->map->Enable();
	App->gui->Enable();
	App->player1->Enable();
	App->player2->Enable();
	App->entitymanager->Enable();
	
}

