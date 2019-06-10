#include "Defs.h"
#include "Log.h"

#include "EntityManager.h"
#include "App.h"
#include "Audio.h"
#include "Render.h"
#include "Pathfinding.h"
#include "Scene.h"
#include "Transitions.h"
#include "Entity.h"
#include "Player.h"
#include "Textures.h"
#include "Barracks.h"
#include "CommandCenter.h"
#include "DefenseAOE.h"
#include "DefenseTarget.h"
#include "MainDefense.h"
#include "Mines.h"
#include "Townhall.h"
#include "Walls.h"
#include "Building.h"
#include "Troop.h"

#include "Brofiler\Brofiler.h"
#include "PugiXml/src/pugixml.hpp"
#include <cmath>

//#include "Engineer.h"
//#include "Tankman.h"
//#include "Infiltrator.h"

EntityManager::EntityManager()
{
	name.append("entitymanager");
}

EntityManager::~EntityManager()
{
}

bool EntityManager::Awake(pugi::xml_node &config)
{
	folder.append(config.child("folder").child_value());
	texture_path = config.child("sprite_sheet").attribute("source").as_string();

	entitiesTextures = vector<SDL_Texture*> (Entity::entityType::WAR_HOUND, nullptr);
	
	return true;
}
bool EntityManager::LoadSamples() {

	// SOLDIER
	string path = "animation/BasicSoldier.tmx";

	soldier_p1_sample.moving = vector<Animation*>(TroopDir::MAX_DIR, nullptr);
	soldier_p1_sample.shooting = vector<Animation*>(TroopDir::MAX_DIR, nullptr);

	soldier_p1_sample.idle = soldier_p1_sample.idle->LoadAnimation(path.data(),"red_idle");

	soldier_p1_sample.moving[NORTH] = soldier_p1_sample.moving[NORTH]		 ->LoadAnimation(path.data(), "red_north"    );
	soldier_p1_sample.moving[SOUTH] = soldier_p1_sample.moving[SOUTH]		 ->LoadAnimation(path.data(), "red_south"    );
	soldier_p1_sample.moving[EAST] = soldier_p1_sample.moving[EAST]			 ->LoadAnimation(path.data(), "red_east"	 );
	soldier_p1_sample.moving[WEST] = soldier_p1_sample.moving[WEST]			 ->LoadAnimation(path.data(), "red_west"     );
	soldier_p1_sample.moving[NORTHEAST] = soldier_p1_sample.moving[NORTHEAST]->LoadAnimation(path.data(), "red_northeast");
	soldier_p1_sample.moving[NORTHWEST] = soldier_p1_sample.moving[NORTHWEST]->LoadAnimation(path.data(), "red_northwest");
	soldier_p1_sample.moving[SOUTHEAST] = soldier_p1_sample.moving[SOUTHEAST]->LoadAnimation(path.data(), "red_southeast");
	soldier_p1_sample.moving[SOUTHWEST] = soldier_p1_sample.moving[SOUTHWEST]->LoadAnimation(path.data(), "red_southwest");

	soldier_p1_sample.shooting[NORTH] = soldier_p1_sample.shooting[NORTH]			->LoadAnimation(path.data(),"red_shoot_N" );
	soldier_p1_sample.shooting[SOUTH] = soldier_p1_sample.shooting[SOUTH]			->LoadAnimation(path.data(),"red_shoot_S" );
	soldier_p1_sample.shooting[EAST] = soldier_p1_sample.shooting[EAST]				->LoadAnimation(path.data(),"red_shoot_E" );
	soldier_p1_sample.shooting[WEST] = soldier_p1_sample.shooting[WEST]				->LoadAnimation(path.data(),"red_shoot_W" );
	soldier_p1_sample.shooting[NORTHEAST] = soldier_p1_sample.shooting[NORTHEAST]	->LoadAnimation(path.data(),"red_shoot_NE");
	soldier_p1_sample.shooting[NORTHWEST] = soldier_p1_sample.shooting[NORTHWEST]	->LoadAnimation(path.data(),"red_shoot_NW");
	soldier_p1_sample.shooting[SOUTHEAST] = soldier_p1_sample.shooting[SOUTHEAST]	->LoadAnimation(path.data(),"red_shoot_SE");
	soldier_p1_sample.shooting[SOUTHWEST] = soldier_p1_sample.shooting[SOUTHWEST]	->LoadAnimation(path.data(),"red_shoot_SW");

	for (int i = NORTH; i <= SOUTHWEST; i++) {
		soldier_p1_sample.moving[i]->speed = 2;
		soldier_p1_sample.shooting[i]->speed = 2;
	}

	soldier_p1_sample.idle->speed = 0;
	soldier_p1_sample.idle->SetCurrentFrame(2);
	soldier_p1_sample.Current_Animation = soldier_p1_sample.moving[NORTH];


	//Player 2
	soldier_p2_sample.moving = vector<Animation*>(TroopDir::MAX_DIR, nullptr);
	soldier_p2_sample.shooting = vector<Animation*>(TroopDir::MAX_DIR, nullptr);

	soldier_p2_sample.idle = soldier_p2_sample.idle->LoadAnimation(path.data(), "blue_idle");

	soldier_p2_sample.moving[NORTH] = soldier_p2_sample.moving[NORTH]			->LoadAnimation(path.data(), "blue_north");
	soldier_p2_sample.moving[SOUTH] = soldier_p2_sample.moving[SOUTH]			->LoadAnimation(path.data(), "blue_south");
	soldier_p2_sample.moving[EAST] = soldier_p2_sample.moving[EAST]				->LoadAnimation(path.data(), "blue_east");
	soldier_p2_sample.moving[WEST] = soldier_p2_sample.moving[WEST]				->LoadAnimation(path.data(), "blue_west");
	soldier_p2_sample.moving[NORTHEAST] = soldier_p2_sample.moving[NORTHEAST]	->LoadAnimation(path.data(), "blue_northeast");
	soldier_p2_sample.moving[NORTHWEST] = soldier_p2_sample.moving[NORTHWEST]	->LoadAnimation(path.data(), "blue_northwest");
	soldier_p2_sample.moving[SOUTHEAST] = soldier_p2_sample.moving[SOUTHEAST]	->LoadAnimation(path.data(), "blue_southeast");
	soldier_p2_sample.moving[SOUTHWEST] = soldier_p2_sample.moving[SOUTHWEST]	->LoadAnimation(path.data(), "blue_southwest");

	soldier_p2_sample.shooting[NORTH] = soldier_p2_sample.shooting[NORTH]->LoadAnimation(path.data(),"blue_shoot_N");
	soldier_p2_sample.shooting[SOUTH] = soldier_p2_sample.shooting[SOUTH]->LoadAnimation(path.data(),"blue_shoot_S");
	soldier_p2_sample.shooting[EAST] = soldier_p2_sample.shooting[EAST]->LoadAnimation(path.data(),	 "blue_shoot_E");
	soldier_p2_sample.shooting[WEST] = soldier_p2_sample.shooting[WEST]->LoadAnimation(path.data(),	 "blue_shoot_W");
	soldier_p2_sample.shooting[NORTHEAST] = soldier_p2_sample.shooting[NORTHEAST]->LoadAnimation(path.data(),"blue_shoot_NE");
	soldier_p2_sample.shooting[NORTHWEST] = soldier_p2_sample.shooting[NORTHWEST]->LoadAnimation(path.data(),"blue_shoot_NW");
	soldier_p2_sample.shooting[SOUTHEAST] = soldier_p2_sample.shooting[SOUTHEAST]->LoadAnimation(path.data(),"blue_shoot_SE");
	soldier_p2_sample.shooting[SOUTHWEST] = soldier_p2_sample.shooting[SOUTHWEST]->LoadAnimation(path.data(),"blue_shoot_SW");

	for (int i = NORTH; i <= SOUTHWEST; i++) {
		soldier_p2_sample.moving[i]->speed = 2;
		soldier_p2_sample.shooting[i]->speed = 2;
	}

	soldier_p2_sample.idle->speed = 0;
	soldier_p2_sample.idle->SetCurrentFrame(2);
	soldier_p2_sample.Current_Animation = soldier_p2_sample.moving[NORTH];

	// Soldier 1 inmune

	soldier_p1_inmune.moving = vector<Animation*>(TroopDir::MAX_DIR, nullptr);
	soldier_p1_inmune.shooting = vector<Animation*>(TroopDir::MAX_DIR, nullptr);

	soldier_p1_inmune.idle = soldier_p1_inmune.idle->LoadAnimation(path.data(), "red_inv_idle");

	soldier_p1_inmune.moving[NORTH] = soldier_p1_inmune.moving[NORTH]->LoadAnimation(path.data(), "red_inv_N");
	soldier_p1_inmune.moving[SOUTH] = soldier_p1_inmune.moving[SOUTH]->LoadAnimation(path.data(), "red_inv_S");
	soldier_p1_inmune.moving[EAST] = soldier_p1_inmune.moving[EAST]->LoadAnimation(path.data(), "red_inv_E");
	soldier_p1_inmune.moving[WEST] = soldier_p1_inmune.moving[WEST]->LoadAnimation(path.data(), "red_inv_W");
	soldier_p1_inmune.moving[NORTHEAST] = soldier_p1_inmune.moving[NORTHEAST]->LoadAnimation(path.data(), "red_inv_NE");
	soldier_p1_inmune.moving[NORTHWEST] = soldier_p1_inmune.moving[NORTHWEST]->LoadAnimation(path.data(), "red_inv_NW");
	soldier_p1_inmune.moving[SOUTHEAST] = soldier_p1_inmune.moving[SOUTHEAST]->LoadAnimation(path.data(), "red_inv_SE");
	soldier_p1_inmune.moving[SOUTHWEST] = soldier_p1_inmune.moving[SOUTHWEST]->LoadAnimation(path.data(), "red_inv_SW");

	soldier_p1_inmune.shooting[NORTH] = soldier_p1_inmune.shooting[NORTH]->LoadAnimation(path.data(), "red_inv_shoot_N");
	soldier_p1_inmune.shooting[SOUTH] = soldier_p1_inmune.shooting[SOUTH]->LoadAnimation(path.data(), "red_inv_shoot_S");
	soldier_p1_inmune.shooting[EAST] = soldier_p1_inmune.shooting[EAST]->LoadAnimation(path.data(), "red_inv_shoot_E");
	soldier_p1_inmune.shooting[WEST] = soldier_p1_inmune.shooting[WEST]->LoadAnimation(path.data(), "red_inv_shoot_W");
	soldier_p1_inmune.shooting[NORTHEAST] = soldier_p1_inmune.shooting[NORTHEAST]->LoadAnimation(path.data(), "red_inv_shoot_NE");
	soldier_p1_inmune.shooting[NORTHWEST] = soldier_p1_inmune.shooting[NORTHWEST]->LoadAnimation(path.data(), "red_inv_shoot_NW");
	soldier_p1_inmune.shooting[SOUTHEAST] = soldier_p1_inmune.shooting[SOUTHEAST]->LoadAnimation(path.data(), "red_inv_shoot_SE");
	soldier_p1_inmune.shooting[SOUTHWEST] = soldier_p1_inmune.shooting[SOUTHWEST]->LoadAnimation(path.data(), "red_inv_shoot_SW");

	for (int i = NORTH; i <= SOUTHWEST; i++) {
		soldier_p1_inmune.moving[i]->speed = 2;
		soldier_p1_inmune.shooting[i]->speed = 2;
	}

	soldier_p1_inmune.idle->speed = 0;
	soldier_p1_inmune.idle->SetCurrentFrame(2);
	soldier_p1_inmune.Current_Animation = soldier_p1_inmune.moving[NORTH];

	//Soldier 2 inmune 

	soldier_p2_inmune.moving = vector<Animation*>(TroopDir::MAX_DIR, nullptr);
	soldier_p2_inmune.shooting = vector<Animation*>(TroopDir::MAX_DIR, nullptr);

	soldier_p2_inmune.idle = soldier_p2_inmune.idle->LoadAnimation(path.data(), "blue_inv_idle");

	soldier_p2_inmune.moving[NORTH] = soldier_p2_inmune.moving[NORTH]->LoadAnimation(path.data(), "blue_inv_N");
	soldier_p2_inmune.moving[SOUTH] = soldier_p2_inmune.moving[SOUTH]->LoadAnimation(path.data(), "blue_inv_S");
	soldier_p2_inmune.moving[EAST] = soldier_p2_inmune.moving[EAST]->LoadAnimation(path.data(), "blue_inv_E");
	soldier_p2_inmune.moving[WEST] = soldier_p2_inmune.moving[WEST]->LoadAnimation(path.data(), "blue_inv_W");
	soldier_p2_inmune.moving[NORTHEAST] = soldier_p2_inmune.moving[NORTHEAST]->LoadAnimation(path.data(), "blue_inv_NE");
	soldier_p2_inmune.moving[NORTHWEST] = soldier_p2_inmune.moving[NORTHWEST]->LoadAnimation(path.data(), "blue_inv_NW");
	soldier_p2_inmune.moving[SOUTHEAST] = soldier_p2_inmune.moving[SOUTHEAST]->LoadAnimation(path.data(), "blue_inv_SE");
	soldier_p2_inmune.moving[SOUTHWEST] = soldier_p2_inmune.moving[SOUTHWEST]->LoadAnimation(path.data(), "blue_inv_SW");

	soldier_p2_inmune.shooting[NORTH] = soldier_p2_inmune.shooting[NORTH]->LoadAnimation(path.data(), "blue_inv_shoot_N");
	soldier_p2_inmune.shooting[SOUTH] = soldier_p2_inmune.shooting[SOUTH]->LoadAnimation(path.data(), "blue_inv_shoot_S");
	soldier_p2_inmune.shooting[EAST] = soldier_p2_inmune.shooting[EAST]->LoadAnimation(path.data(), "blue_inv_shoot_E");
	soldier_p2_inmune.shooting[WEST] = soldier_p2_inmune.shooting[WEST]->LoadAnimation(path.data(), "blue_inv_shoot_W");
	soldier_p2_inmune.shooting[NORTHEAST] = soldier_p2_inmune.shooting[NORTHEAST]->LoadAnimation(path.data(), "blue_inv_shoot_NE");
	soldier_p2_inmune.shooting[NORTHWEST] = soldier_p2_inmune.shooting[NORTHWEST]->LoadAnimation(path.data(), "blue_inv_shoot_NW");
	soldier_p2_inmune.shooting[SOUTHEAST] = soldier_p2_inmune.shooting[SOUTHEAST]->LoadAnimation(path.data(), "blue_inv_shoot_SE");
	soldier_p2_inmune.shooting[SOUTHWEST] = soldier_p2_inmune.shooting[SOUTHWEST]->LoadAnimation(path.data(), "blue_inv_shoot_SW");

	for (int i = NORTH; i <= SOUTHWEST; i++) {
		soldier_p2_inmune.moving[i]->speed = 2;
		soldier_p2_inmune.shooting[i]->speed = 2;
	}

	soldier_p2_inmune.idle->speed = 0;
	soldier_p2_inmune.idle->SetCurrentFrame(2);
	soldier_p2_inmune.Current_Animation = soldier_p2_inmune.moving[NORTH];

	// END SOLDIER -------------------------

	

	// ENGINEER ---------------

	path = "animation/engineer.tmx";

	engineer_p1_sample.moving = vector<Animation*>(TroopDir::MAX_DIR, nullptr);
	engineer_p1_sample.shooting = vector<Animation*>(TroopDir::MAX_DIR, nullptr);

	engineer_p1_sample.idle = engineer_p1_sample.idle->LoadAnimation(path.data(), "red_idle");

	engineer_p1_sample.moving[NORTH] = engineer_p1_sample.moving[NORTH]->LoadAnimation(path.data(),			"red_run_N" );
	engineer_p1_sample.moving[SOUTH] = engineer_p1_sample.moving[SOUTH]->LoadAnimation(path.data(),			"red_run_S" );
	engineer_p1_sample.moving[EAST] = engineer_p1_sample.moving[EAST]->LoadAnimation(path.data(),			"red_run_E" );
	engineer_p1_sample.moving[WEST] = engineer_p1_sample.moving[WEST]->LoadAnimation(path.data(),			"red_run_W" );
	engineer_p1_sample.moving[NORTHEAST] = engineer_p1_sample.moving[NORTHEAST]->LoadAnimation(path.data(), "red_run_NE");
	engineer_p1_sample.moving[NORTHWEST] = engineer_p1_sample.moving[NORTHWEST]->LoadAnimation(path.data(), "red_run_NW");
	engineer_p1_sample.moving[SOUTHEAST] = engineer_p1_sample.moving[SOUTHEAST]->LoadAnimation(path.data(), "red_run_SE");
	engineer_p1_sample.moving[SOUTHWEST] = engineer_p1_sample.moving[SOUTHWEST]->LoadAnimation(path.data(), "red_run_SW");

	engineer_p1_sample.shooting[NORTH] = engineer_p1_sample.shooting[NORTH]->LoadAnimation(path.data(),			"red_search_N" );
	engineer_p1_sample.shooting[SOUTH] = engineer_p1_sample.shooting[SOUTH]->LoadAnimation(path.data(),			"red_search_S" );
	engineer_p1_sample.shooting[EAST] = engineer_p1_sample.shooting[EAST]->LoadAnimation(path.data(),			"red_search_E" );
	engineer_p1_sample.shooting[WEST] = engineer_p1_sample.shooting[WEST]->LoadAnimation(path.data(),			"red_search_W" );
	engineer_p1_sample.shooting[NORTHEAST] = engineer_p1_sample.shooting[NORTHEAST]->LoadAnimation(path.data(), "red_search_NE");
	engineer_p1_sample.shooting[NORTHWEST] = engineer_p1_sample.shooting[NORTHWEST]->LoadAnimation(path.data(), "red_search_NW");
	engineer_p1_sample.shooting[SOUTHEAST] = engineer_p1_sample.shooting[SOUTHEAST]->LoadAnimation(path.data(), "red_search_SE");
	engineer_p1_sample.shooting[SOUTHWEST] = engineer_p1_sample.shooting[SOUTHWEST]->LoadAnimation(path.data(), "red_search_SW");

	for (int i = NORTH; i <= SOUTHWEST; i++) {
		engineer_p1_sample.moving[i]->speed = 2;
		engineer_p1_sample.shooting[i]->speed = 2;
	}

	engineer_p1_sample.idle->speed = 0;
	engineer_p1_sample.idle->SetCurrentFrame(2);
	engineer_p1_sample.Current_Animation = engineer_p1_sample.moving[NORTH];


	//Player 2
	engineer_p2_sample.moving = vector<Animation*>(TroopDir::MAX_DIR, nullptr);
	engineer_p2_sample.shooting = vector<Animation*>(TroopDir::MAX_DIR, nullptr);

	engineer_p2_sample.idle = engineer_p2_sample.idle->LoadAnimation(path.data(), "blue_idle");

	engineer_p2_sample.moving[NORTH] = engineer_p2_sample.moving[NORTH]->LoadAnimation(path.data(),		"blue_run_N");
	engineer_p2_sample.moving[SOUTH] = engineer_p2_sample.moving[SOUTH]->LoadAnimation(path.data(),		"blue_run_S");
	engineer_p2_sample.moving[EAST] = engineer_p2_sample.moving[EAST]->LoadAnimation(path.data(),		"blue_run_E");
	engineer_p2_sample.moving[WEST] = engineer_p2_sample.moving[WEST]->LoadAnimation(path.data(),		"blue_run_W");
	engineer_p2_sample.moving[NORTHEAST] = engineer_p2_sample.moving[NORTHEAST]->LoadAnimation(path.data(), "blue_run_NE");
	engineer_p2_sample.moving[NORTHWEST] = engineer_p2_sample.moving[NORTHWEST]->LoadAnimation(path.data(), "blue_run_NW");
	engineer_p2_sample.moving[SOUTHEAST] = engineer_p2_sample.moving[SOUTHEAST]->LoadAnimation(path.data(), "blue_run_SE");
	engineer_p2_sample.moving[SOUTHWEST] = engineer_p2_sample.moving[SOUTHWEST]->LoadAnimation(path.data(), "blue_run_SW");

	engineer_p2_sample.shooting[NORTH] = engineer_p2_sample.shooting[NORTH]->LoadAnimation(path.data(),		"blue_search_N");
	engineer_p2_sample.shooting[SOUTH] = engineer_p2_sample.shooting[SOUTH]->LoadAnimation(path.data(),		"blue_search_S");
	engineer_p2_sample.shooting[EAST] = engineer_p2_sample.shooting[EAST]->LoadAnimation(path.data(),		"blue_search_E");
	engineer_p2_sample.shooting[WEST] = engineer_p2_sample.shooting[WEST]->LoadAnimation(path.data(),		"blue_search_W");
	engineer_p2_sample.shooting[NORTHEAST] = engineer_p2_sample.shooting[NORTHEAST]->LoadAnimation(path.data(), "blue_search_NE");
	engineer_p2_sample.shooting[NORTHWEST] = engineer_p2_sample.shooting[NORTHWEST]->LoadAnimation(path.data(), "blue_search_NW");
	engineer_p2_sample.shooting[SOUTHEAST] = engineer_p2_sample.shooting[SOUTHEAST]->LoadAnimation(path.data(), "blue_search_SE");
	engineer_p2_sample.shooting[SOUTHWEST] = engineer_p2_sample.shooting[SOUTHWEST]->LoadAnimation(path.data(), "blue_search_SW");

	for (int i = NORTH; i <= SOUTHWEST; i++) {
		engineer_p2_sample.moving[i]->speed = 2;
		engineer_p2_sample.shooting[i]->speed = 2;
	}

	engineer_p2_sample.idle->speed = 0;
	engineer_p2_sample.idle->SetCurrentFrame(2);
	engineer_p2_sample.Current_Animation = engineer_p2_sample.moving[NORTH];


	// END ENGINEER  -------------

	// HOUND

	path = "animation/war_hound.tmx";

	hound_p1_sample.moving = vector<Animation*>(TroopDir::MAX_DIR, nullptr);
	hound_p1_sample.shooting = vector<Animation*>(TroopDir::MAX_DIR, nullptr);

	hound_p1_sample.idle = hound_p1_sample.idle->LoadAnimation(path.data(), "idle");

	hound_p1_sample.moving[NORTH] = hound_p1_sample.moving[NORTH]->LoadAnimation(path.data(),		"run_N" );
	hound_p1_sample.moving[SOUTH] = hound_p1_sample.moving[SOUTH]->LoadAnimation(path.data(),		"run_S" );
	hound_p1_sample.moving[EAST] = hound_p1_sample.moving[EAST]->LoadAnimation(path.data(),			"run_E" );
	hound_p1_sample.moving[WEST] = hound_p1_sample.moving[WEST]->LoadAnimation(path.data(),			"run_W" );
	hound_p1_sample.moving[NORTHEAST] = hound_p1_sample.moving[NORTHEAST]->LoadAnimation(path.data(),"run_NE");
	hound_p1_sample.moving[NORTHWEST] = hound_p1_sample.moving[NORTHWEST]->LoadAnimation(path.data(),"run_NW");
	hound_p1_sample.moving[SOUTHEAST] = hound_p1_sample.moving[SOUTHEAST]->LoadAnimation(path.data(),"run_SE");
	hound_p1_sample.moving[SOUTHWEST] = hound_p1_sample.moving[SOUTHWEST]->LoadAnimation(path.data(),"run_SW");

	hound_p1_sample.shooting[NORTH] = hound_p1_sample.shooting[NORTH]->LoadAnimation(path.data(),			"attack_N" );
	hound_p1_sample.shooting[SOUTH] = hound_p1_sample.shooting[SOUTH]->LoadAnimation(path.data(),			"attack_S" );
	hound_p1_sample.shooting[EAST] = hound_p1_sample.shooting[EAST]->LoadAnimation(path.data(),				"attack_E" );
	hound_p1_sample.shooting[WEST] = hound_p1_sample.shooting[WEST]->LoadAnimation(path.data(),				"attack_W" );
	hound_p1_sample.shooting[NORTHEAST] = hound_p1_sample.shooting[NORTHEAST]->LoadAnimation(path.data(),	"attack_NE");
	hound_p1_sample.shooting[NORTHWEST] = hound_p1_sample.shooting[NORTHWEST]->LoadAnimation(path.data(),	"attack_NW");
	hound_p1_sample.shooting[SOUTHEAST] = hound_p1_sample.shooting[SOUTHEAST]->LoadAnimation(path.data(),	"attack_SE");
	hound_p1_sample.shooting[SOUTHWEST] = hound_p1_sample.shooting[SOUTHWEST]->LoadAnimation(path.data(),	"attack_SW");

	for (int i = NORTH; i <= SOUTHWEST; i++) {
		hound_p1_sample.moving[i]->speed = 2;
		hound_p1_sample.shooting[i]->speed = 2;
	}

	hound_p1_sample.idle->speed = 0;
	hound_p1_sample.idle->SetCurrentFrame(2);
	hound_p1_sample.Current_Animation = hound_p1_sample.moving[NORTH];


	//Player 2
	hound_p2_sample.moving = vector<Animation*>(TroopDir::MAX_DIR, nullptr);
	hound_p2_sample.shooting = vector<Animation*>(TroopDir::MAX_DIR, nullptr);

	hound_p2_sample.idle = hound_p2_sample.idle->LoadAnimation(path.data(), "idle_blue");

	hound_p2_sample.moving[NORTH] = hound_p2_sample.moving[NORTH]->LoadAnimation(path.data(),		  "blue_run_N");
	hound_p2_sample.moving[SOUTH] = hound_p2_sample.moving[SOUTH]->LoadAnimation(path.data(),		  "blue_run_S");
	hound_p2_sample.moving[EAST] = hound_p2_sample.moving[EAST]->LoadAnimation(path.data(),			  "blue_run_E");
	hound_p2_sample.moving[WEST] = hound_p2_sample.moving[WEST]->LoadAnimation(path.data(),			  "blue_run_W");
	hound_p2_sample.moving[NORTHEAST] = hound_p2_sample.moving[NORTHEAST]->LoadAnimation(path.data(),  "blue_run_NE");
	hound_p2_sample.moving[NORTHWEST] = hound_p2_sample.moving[NORTHWEST]->LoadAnimation(path.data(),  "blue_run_NW");
	hound_p2_sample.moving[SOUTHEAST] = hound_p2_sample.moving[SOUTHEAST]->LoadAnimation(path.data(),  "blue_run_SE");
	hound_p2_sample.moving[SOUTHWEST] = hound_p2_sample.moving[SOUTHWEST]->LoadAnimation(path.data(),  "blue_run_SW");

	hound_p2_sample.shooting[NORTH] = hound_p2_sample.shooting[NORTH]->LoadAnimation(path.data(),		"blue_attack_N");
	hound_p2_sample.shooting[SOUTH] = hound_p2_sample.shooting[SOUTH]->LoadAnimation(path.data(),		"blue_attack_S");
	hound_p2_sample.shooting[EAST] = hound_p2_sample.shooting[EAST]->LoadAnimation(path.data(),			"blue_attack_E");
	hound_p2_sample.shooting[WEST] = hound_p2_sample.shooting[WEST]->LoadAnimation(path.data(),			"blue_attack_W");
	hound_p2_sample.shooting[NORTHEAST] = hound_p2_sample.shooting[NORTHEAST]->LoadAnimation(path.data(), "blue_attack_NE");
	hound_p2_sample.shooting[NORTHWEST] = hound_p2_sample.shooting[NORTHWEST]->LoadAnimation(path.data(), "blue_attack_NW");
	hound_p2_sample.shooting[SOUTHEAST] = hound_p2_sample.shooting[SOUTHEAST]->LoadAnimation(path.data(), "blue_attack_SE");
	hound_p2_sample.shooting[SOUTHWEST] = hound_p2_sample.shooting[SOUTHWEST]->LoadAnimation(path.data(), "blue_attack_SW");

	for (int i = NORTH; i <= SOUTHWEST; i++) {
		hound_p2_sample.moving[i]->speed = 2;
		hound_p2_sample.shooting[i]->speed = 2;
	}

	hound_p2_sample.idle->speed = 0;
	hound_p2_sample.idle->SetCurrentFrame(2);
	hound_p2_sample.Current_Animation = hound_p2_sample.moving[NORTH];

	// END HOUND -------------

	// SPY

	path = "animation/infiltrator.tmx"; 

	infiltrator_p1_sample.moving = vector<Animation*>(TroopDir::MAX_DIR, nullptr);
	infiltrator_p1_sample.shooting = vector<Animation*>(TroopDir::MAX_DIR, nullptr);

	infiltrator_p1_sample.idle = infiltrator_p1_sample.idle->LoadAnimation(path.data(), "idle");

	infiltrator_p1_sample.moving[NORTH] = infiltrator_p1_sample.moving[NORTH]->LoadAnimation(path.data(), "run_N");
	infiltrator_p1_sample.moving[SOUTH] = infiltrator_p1_sample.moving[SOUTH]->LoadAnimation(path.data(), "run_S");
	infiltrator_p1_sample.moving[EAST] = infiltrator_p1_sample.moving[EAST]->LoadAnimation(path.data(), "run_E");
	infiltrator_p1_sample.moving[WEST] = infiltrator_p1_sample.moving[WEST]->LoadAnimation(path.data(), "run_W");
	infiltrator_p1_sample.moving[NORTHEAST] = infiltrator_p1_sample.moving[NORTHEAST]->LoadAnimation(path.data(), "run_NE");
	infiltrator_p1_sample.moving[NORTHWEST] = infiltrator_p1_sample.moving[NORTHWEST]->LoadAnimation(path.data(), "run_NW");
	infiltrator_p1_sample.moving[SOUTHEAST] = infiltrator_p1_sample.moving[SOUTHEAST]->LoadAnimation(path.data(), "run_SE");
	infiltrator_p1_sample.moving[SOUTHWEST] = infiltrator_p1_sample.moving[SOUTHWEST]->LoadAnimation(path.data(), "run_SW");

	infiltrator_p1_sample.shooting[NORTH] = infiltrator_p1_sample.shooting[NORTH]->LoadAnimation(path.data(), "searching_N");
	infiltrator_p1_sample.shooting[SOUTH] = infiltrator_p1_sample.shooting[SOUTH]->LoadAnimation(path.data(), "searching_S");
	infiltrator_p1_sample.shooting[EAST] = infiltrator_p1_sample.shooting[EAST]->LoadAnimation(path.data(), "searching_E");
	infiltrator_p1_sample.shooting[WEST] = infiltrator_p1_sample.shooting[WEST]->LoadAnimation(path.data(), "searching_W");
	infiltrator_p1_sample.shooting[NORTHEAST] = infiltrator_p1_sample.shooting[NORTHEAST]->LoadAnimation(path.data(), "searching_N");
	infiltrator_p1_sample.shooting[NORTHWEST] = infiltrator_p1_sample.shooting[NORTHWEST]->LoadAnimation(path.data(), "searching_N");
	infiltrator_p1_sample.shooting[SOUTHEAST] = infiltrator_p1_sample.shooting[SOUTHEAST]->LoadAnimation(path.data(), "searching_S");
	infiltrator_p1_sample.shooting[SOUTHWEST] = infiltrator_p1_sample.shooting[SOUTHWEST]->LoadAnimation(path.data(), "searching_S");

	for (int i = NORTH; i <= SOUTHWEST; i++) {
		infiltrator_p1_sample.moving[i]->speed = 2;
		infiltrator_p1_sample.shooting[i]->speed = 2;
	}

	infiltrator_p1_sample.idle->speed = 0;
	infiltrator_p1_sample.idle->SetCurrentFrame(2);
	infiltrator_p1_sample.Current_Animation = infiltrator_p1_sample.moving[NORTH];

	// Spy inmune

	infiltrator_p1_inmune.moving = vector<Animation*>(TroopDir::MAX_DIR, nullptr);
	infiltrator_p1_inmune.shooting = vector<Animation*>(TroopDir::MAX_DIR, nullptr);

	infiltrator_p1_inmune.idle = infiltrator_p1_inmune.idle->LoadAnimation(path.data(), "inv_idle");

	infiltrator_p1_inmune.moving[NORTH] = infiltrator_p1_inmune.moving[NORTH]->LoadAnimation(path.data(), "inv_run_N");
	infiltrator_p1_inmune.moving[SOUTH] = infiltrator_p1_inmune.moving[SOUTH]->LoadAnimation(path.data(), "inv_run_S");
	infiltrator_p1_inmune.moving[EAST] = infiltrator_p1_inmune.moving[EAST]->LoadAnimation(path.data(), "inv_run_E");
	infiltrator_p1_inmune.moving[WEST] = infiltrator_p1_inmune.moving[WEST]->LoadAnimation(path.data(), "inv_run_W");
	infiltrator_p1_inmune.moving[NORTHEAST] = infiltrator_p1_inmune.moving[NORTHEAST]->LoadAnimation(path.data(), "inv_run_NE");
	infiltrator_p1_inmune.moving[NORTHWEST] = infiltrator_p1_inmune.moving[NORTHWEST]->LoadAnimation(path.data(), "inv_run_NW");
	infiltrator_p1_inmune.moving[SOUTHEAST] = infiltrator_p1_inmune.moving[SOUTHEAST]->LoadAnimation(path.data(), "inv_run_SE");
	infiltrator_p1_inmune.moving[SOUTHWEST] = infiltrator_p1_inmune.moving[SOUTHWEST]->LoadAnimation(path.data(), "inv_run_SW");

	infiltrator_p1_inmune.shooting[NORTH] = infiltrator_p1_inmune.shooting[NORTH]->LoadAnimation(path.data(), "inv_searching_N");
	infiltrator_p1_inmune.shooting[SOUTH] = infiltrator_p1_inmune.shooting[SOUTH]->LoadAnimation(path.data(), "inv_searching_S");
	infiltrator_p1_inmune.shooting[EAST] = infiltrator_p1_inmune.shooting[EAST]->LoadAnimation(path.data(), "inv_searching_E");
	infiltrator_p1_inmune.shooting[WEST] = infiltrator_p1_inmune.shooting[WEST]->LoadAnimation(path.data(), "inv_searching_W");
	infiltrator_p1_inmune.shooting[NORTHEAST] = infiltrator_p1_inmune.shooting[NORTHEAST]->LoadAnimation(path.data(), "inv_searching_N");
	infiltrator_p1_inmune.shooting[NORTHWEST] = infiltrator_p1_inmune.shooting[NORTHWEST]->LoadAnimation(path.data(), "inv_searching_N");
	infiltrator_p1_inmune.shooting[SOUTHEAST] = infiltrator_p1_inmune.shooting[SOUTHEAST]->LoadAnimation(path.data(), "inv_searching_S");
	infiltrator_p1_inmune.shooting[SOUTHWEST] = infiltrator_p1_inmune.shooting[SOUTHWEST]->LoadAnimation(path.data(), "inv_searching_S");

	for (int i = NORTH; i <= SOUTHWEST; i++) {
		infiltrator_p1_inmune.moving[i]->speed = 2;
		infiltrator_p1_inmune.shooting[i]->speed = 2;
	}

	infiltrator_p1_inmune.idle->speed = 0;
	infiltrator_p1_inmune.idle->SetCurrentFrame(2);
	infiltrator_p1_inmune.Current_Animation = infiltrator_p1_inmune.moving[NORTH];

	return true;
}
bool EntityManager::Start()
{
	LoadSamples();
	bool ret = true;
	for (int i = Entity::entityType::TOWNHALL; i <= Entity::entityType::WAR_HOUND; i++) {

		string n = GetName(Entity::entityType(i));
		n += "_anim.png";

		entitiesTextures[i] = App->tex->Load(PATH(folder.data(), n.data()));
	}

	return ret;
}
bool EntityManager::PreUpdate()
{
	BROFILER_CATEGORY("EntityManager PostUpdate", Profiler::Color::Blue);

	bool ret = true;

	
	if (App->scene->pause == false)
	{
		// Player 1 Buildings
		list<Building*>::const_iterator tmp = App->player1->buildings.begin();
		while (tmp != App->player1->buildings.end())
		{
			ret = (*tmp)->PreUpdate();
			tmp++;
		}

		// Player 2 Buildings
		tmp = App->player2->buildings.begin();
		while (tmp != App->player2->buildings.end())
		{
			ret = (*tmp)->PreUpdate();
			tmp++;
		}
		// Player 1 Troops
		list<Troop*>::const_iterator ttmp = App->player1->troops.begin();
		while (ttmp != App->player1->troops.end())
		{
			ret = (*ttmp)->PreUpdate();
			ttmp++;
		}

		// Player 2 Troops
		ttmp = App->player2->troops.begin();
		while (ttmp != App->player2->troops.end())
		{
			ret = (*ttmp)->PreUpdate();
			ttmp++;
		}


	}

	return ret;
}


bool EntityManager::Update(float dt)
{
	BROFILER_CATEGORY("EntityManager Update", Profiler::Color::Blue);

	bool ret = true;

	if (App->scene->pause == false)
	{
		

		// Player 1 Buildings
		list<Building*>::const_iterator tmp = App->player1->buildings.begin();
		while (tmp != App->player1->buildings.end())
		{
			ret = (*tmp)->Update(dt);
			tmp++;
		}

		// Player 2 Buildings
		tmp = App->player2->buildings.begin();
		while (tmp != App->player2->buildings.end())
		{
			ret = (*tmp)->Update(dt);
			tmp++;
		}

		pair<int, int> mouse_pos;
		App->input->GetMousePosition(mouse_pos.first, mouse_pos.second);
		mouse_pos = App->render->ScreenToWorld(mouse_pos.first, mouse_pos.second);

		// Player 1 Troops

		list<Troop*>::const_iterator ttmp = App->player1->troops.begin();

		if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN && ttmp != App->player1->troops.end()) {
			pathfinding = true;
		}
		
		/*if (pathfinding && ttmp != App->player1->troops.end() && (*ttmp)->info.current_group->IsGroupLead(*ttmp)) {
			pathfinding = !App->move_manager->Move((*ttmp)->info.current_group, dt,mouse_pos);
		}	*/	

		
		while (ttmp != App->player1->troops.end())
		{
			ret = (*ttmp)->Update(dt);
			ttmp++;
		}

		// Player 2 Troops
		ttmp = App->player2->troops.begin();

		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN && ttmp != App->player2->troops.end()) {
			pathfinding2 = true;
		}

		/*if (pathfinding2 && ttmp != App->player2->troops.end()) {
			pathfinding2 = !App->move_manager->Move((*ttmp)->info.current_group, dt, mouse_pos);
		}*/

		while (ttmp != App->player2->troops.end())
		{
			ret = (*ttmp)->Update(dt);
			ttmp++;
		}

	}

	return ret;
}

bool EntityManager::PostUpdate()
{
	BROFILER_CATEGORY("EntityManager PostUpdate", Profiler::Color::Blue);

	bool ret = true;

	if (App->scene->pause == false)
	{
		// Player 1 Buildings
		list<Building*>::const_iterator tmp = App->player1->buildings.begin();
		while (tmp != App->player1->buildings.end())
		{
			ret = (*tmp)->PostUpdate();
			tmp++;
		}

		// Player 2 Buildings
		tmp = App->player2->buildings.begin();
		while (tmp != App->player2->buildings.end())
		{
			ret = (*tmp)->PostUpdate();
			tmp++;
		}
		// Player 1 Troops
		list<Troop*>::const_iterator ttmp = App->player1->troops.begin();
		while (ttmp != App->player1->troops.end())
		{
			ret = (*ttmp)->PostUpdate();
			ttmp++;
		}

		// Player 2 Troops
		ttmp = App->player2->troops.begin();
		while (ttmp != App->player2->troops.end())
		{
			ret = (*ttmp)->PostUpdate();
			ttmp++;
		}


	}

	return ret;
}

bool EntityManager::CleanUp()
{
	App->tex->UnLoad(texture);
	entity_list.clear();

	for (int i = Entity::entityType::TOWNHALL; i < Entity::entityType::WAR_HOUND; i++) {

			if (entitiesTextures[i] = nullptr) {
				App->tex->UnLoad(entitiesTextures[i]);
			}
	}

	//Building::CleanUp();
	return true;
}

void EntityManager::DeleteAllEntities()
{
	// Main entity list
	list<Entity*>::iterator	tmp = entity_list.begin();
	while (tmp != entity_list.end())
	{
		(*tmp)->CleanUp();
		RELEASE(*tmp);
		tmp++;
	}
	entity_list.clear();


	// Player 1 Buildings
	list<Building*>::iterator btmp = App->player1->buildings.begin();
	while (btmp != App->player1->buildings.end())
	{
		(*btmp)->CleanUp();
		RELEASE(*btmp);
		btmp++;
	}
	App->player1->buildings.clear();

	// Player 2 Buildings
	btmp = App->player2->buildings.begin();
	while (btmp != App->player2->buildings.end())
	{
		(*btmp)->CleanUp();
		RELEASE(*btmp);
		btmp++;
	}
	App->player2->buildings.clear();

	// Player 1 Troops
	list<Troop*>::iterator ttmp = App->player1->troops.begin();
	while (ttmp != App->player1->troops.end())
	{
		(*ttmp)->CleanUp();
		RELEASE(*ttmp);
		ttmp++;
	}
	App->player1->troops.clear();

	// Player 2 Troops
	ttmp = App->player2->troops.begin();
	while (ttmp != App->player2->troops.end())
	{
		(*ttmp)->CleanUp();
		RELEASE(*ttmp);
		ttmp++;
	}
	App->player2->troops.clear();

}

bool EntityManager::Draw(float dt) //sprite ordering
{
	bool ret = true;

	list<Entity*>::iterator tmp = entity_list.begin();

	while (tmp != entity_list.end())
	{

		//if (entitiesTextures[(*tmp)->type] != nullptr) {

		//	int posy = (*tmp)->position.second - (*tmp)->Current_Animation->GetCurrentFrame(dt).h;// - ((*tmp)->Current_Animation->GetCurrentFrame(dt).h - (*tmp)->position.second);
		//	App->render->Blit(entitiesTextures[(*tmp)->type],  (*tmp)->position.first ,posy, &((*tmp)->Current_Animation->GetCurrentFrame(dt)), SDL_FLIP_NONE);
		//	
		//	pair<int,int> pos = App->map->WorldToMap((*tmp)->position.first /*- (*tmp)->size.first * App->map->data.tile_width*0.5f*/, (*tmp)->position.second - (*tmp)->size.second*App->map->data.tile_height*0.5f);
		//	pos = App->map->MapToWorld(pos.first, pos.second);

		//	pair<int, int> pos = { (*tmp)->position.first,(*tmp)->position.second - (*tmp)->offset };
		//	App->render->Blit(entitiesTextures[(*tmp)->type], pos.first, pos.second, &((*tmp)->Current_Animation->GetCurrentFrame(dt)), SDL_FLIP_NONE);

		//}
		//if (entitiesTextures[(*tmp)->type] != nullptr)


		if ((*tmp)->type == Entity::entityType::TOWNHALL && (*tmp)->fromPlayer1 == true)
		{
			App->render->Blit(entitiesTextures[(*tmp)->type], (*tmp)->position.first - ((*tmp)->collider.dimensions.first * 8), 
				(*tmp)->position.second - (*tmp)->Current_Animation->frames->h + ((*tmp)->collider.dimensions.second * 40), 
				&((*tmp)->Current_Animation->GetCurrentFrame(dt)));


			//--- Draw Life Bar
			if ((*tmp)->health < (*tmp)->health_lv[(*tmp)->level] && (*tmp)->health > 0)
			{
				SDL_Rect rect, rect_bg;

				rect_bg.w = 30;
				rect.w = rect_bg.w * (*tmp)->health / (*tmp)->health_lv[(*tmp)->level];

				rect_bg.h = rect.h = 5;
				rect_bg.x = rect.x = (*tmp)->position.first - ((*tmp)->collider.dimensions.first * 20) + ((*tmp)->Current_Animation->GetCurrentFrame(dt).w / 2) - (rect_bg.w / 1.5) + 70;
				rect_bg.y = rect.y = (*tmp)->position.second - (*tmp)->Current_Animation->frames->h + ((*tmp)->collider.dimensions.second * 20) + 70;

				App->render->DrawQuad(rect_bg, 255, 0, 0, 255); //background (red)
				App->render->DrawQuad(rect, 0, 255, 0, 255); //life (green)
			}

		}
		else if (((*tmp)->type == Entity::entityType::TOWNHALL && (*tmp)->fromPlayer1 == false))
		{
			App->render->Blit(entitiesTextures[(*tmp)->type], (*tmp)->position.first - ((*tmp)->collider.dimensions.first * 20), (*tmp)->position.second - (*tmp)->Current_Animation->frames->h + ((*tmp)->collider.dimensions.second * 20), &((*tmp)->Current_Animation->GetCurrentFrame(dt)));

			//--- Draw Life Bar
			if ((*tmp)->health < (*tmp)->health_lv[(*tmp)->level] && (*tmp)->health > 0)
			{
				SDL_Rect rect, rect_bg;

				rect_bg.w = 30;
				rect.w = rect_bg.w * (*tmp)->health / (*tmp)->health_lv[(*tmp)->level];

				rect_bg.h = rect.h = 5;
				rect_bg.x = rect.x = (*tmp)->position.first - ((*tmp)->collider.dimensions.first * 8) + ((*tmp)->Current_Animation->GetCurrentFrame(dt).w / 2) - (rect_bg.w / 1.5);
				rect_bg.y = rect.y = (*tmp)->position.second - (*tmp)->Current_Animation->frames->h + ((*tmp)->collider.dimensions.second * 40) - 50;

				App->render->DrawQuad(rect_bg, 255, 0, 0, 255); //background (red)
				App->render->DrawQuad(rect, 0, 255, 0, 255); //life (green)
			}
		}
		else if ((*tmp)->type == Entity::entityType::BARRACKS)
		{
			App->render->Blit(entitiesTextures[(*tmp)->type], (*tmp)->position.first - ((*tmp)->collider.dimensions.first * 20) + 30, (*tmp)->position.second - (*tmp)->Current_Animation->frames->h + ((*tmp)->collider.dimensions.second * 20) - 10, &((*tmp)->Current_Animation->GetCurrentFrame(dt)));

			//--- Draw Life Bar
			if ((*tmp)->health < (*tmp)->health_lv[(*tmp)->level] && (*tmp)->health > 0)
			{
				SDL_Rect rect, rect_bg;

				rect_bg.w = 30;
				rect.w = rect_bg.w * (*tmp)->health / (*tmp)->health_lv[(*tmp)->level];

				rect_bg.h = rect.h = 5;
				rect_bg.x = rect.x = (*tmp)->position.first - ((*tmp)->collider.dimensions.first * 20) + ((*tmp)->Current_Animation->GetCurrentFrame(dt).w / 2) - (rect_bg.w / 1.5);
				rect_bg.y = rect.y = (*tmp)->position.second - (*tmp)->Current_Animation->frames->h + ((*tmp)->collider.dimensions.second * 20) - 10;

				App->render->DrawQuad(rect_bg, 255, 0, 0, 255); //background (red)
				App->render->DrawQuad(rect, 0, 255, 0, 255); //life (green)
			}
		}
		else if ((*tmp)->type == Entity::entityType::MINES)
		{
			App->render->Blit(entitiesTextures[(*tmp)->type], (*tmp)->position.first - ((*tmp)->collider.dimensions.first * 20), (*tmp)->position.second - (*tmp)->Current_Animation->frames->h + ((*tmp)->collider.dimensions.second * 20), &((*tmp)->Current_Animation->GetCurrentFrame(dt)));

			//--- Draw Life Bar
			if ((*tmp)->health < (*tmp)->health_lv[(*tmp)->level] && (*tmp)->health > 0)
			{
				SDL_Rect rect, rect_bg;

				rect_bg.w = 30;
				rect.w = rect_bg.w * (*tmp)->health / (*tmp)->health_lv[(*tmp)->level];

				rect_bg.h = rect.h = 5;
				rect_bg.x = rect.x = (*tmp)->position.first - ((*tmp)->collider.dimensions.first * 20) + ((*tmp)->Current_Animation->GetCurrentFrame(dt).w / 2) - (rect_bg.w / 1.5);
				rect_bg.y = rect.y = (*tmp)->position.second - (*tmp)->Current_Animation->frames->h + ((*tmp)->collider.dimensions.second * 20) - 10;

				App->render->DrawQuad(rect_bg, 255, 0, 0, 255); //background (red)
				App->render->DrawQuad(rect, 0, 255, 0, 255); //life (green)
			}
		}
		else if ((*tmp)->type == Entity::entityType::DEFENSE_AOE)
		{
			App->render->Blit(entitiesTextures[(*tmp)->type], (*tmp)->position.first - ((*tmp)->collider.dimensions.first * 20) + 40, (*tmp)->position.second - (*tmp)->Current_Animation->frames->h + ((*tmp)->collider.dimensions.second * 20) - 10, &((*tmp)->Current_Animation->GetCurrentFrame(dt)));


			//--- Draw Life Bar
			if ((*tmp)->health < (*tmp)->health_lv[(*tmp)->level] && (*tmp)->health > 0)
			{
				SDL_Rect rect, rect_bg;

				rect_bg.w = 30;
				rect.w = rect_bg.w * (*tmp)->health / (*tmp)->health_lv[(*tmp)->level];

				rect_bg.h = rect.h = 5;
				rect_bg.x = rect.x = (*tmp)->position.first - ((*tmp)->collider.dimensions.first * 20) + ((*tmp)->Current_Animation->GetCurrentFrame(dt).w / 2) - (rect_bg.w / 1.5);
				rect_bg.y = rect.y = (*tmp)->position.second - (*tmp)->Current_Animation->frames->h + ((*tmp)->collider.dimensions.second * 20) - 10;

				App->render->DrawQuad(rect_bg, 255, 0, 0, 255); //background (red)
				App->render->DrawQuad(rect, 0, 255, 0, 255); //life (green)
			}
		}
		else if ((*tmp)->type == Entity::entityType::COMMAND_CENTER && (*tmp)->fromPlayer1 == true)
		{
			App->render->Blit(entitiesTextures[(*tmp)->type], (*tmp)->position.first/*-((*tmp)->collider.dimensions.first*29)*/, (*tmp)->position.second - (*tmp)->Current_Animation->frames->h + ((*tmp)->collider.dimensions.second * 20), &((*tmp)->Current_Animation->GetCurrentFrame(dt)));

			//--- Draw Life Bar
			if ((*tmp)->health < (*tmp)->health_lv[(*tmp)->level] && (*tmp)->health > 0)
			{
				SDL_Rect rect, rect_bg;

				rect_bg.w = 30;
				rect.w = rect_bg.w * (*tmp)->health / (*tmp)->health_lv[(*tmp)->level];

				rect_bg.h = rect.h = 5;
				rect_bg.x = rect.x = (*tmp)->position.first + ((*tmp)->Current_Animation->GetCurrentFrame(dt).w / 2) - (rect_bg.w / 1.5) + 30 ;
				rect_bg.y = rect.y = (*tmp)->position.second - (*tmp)->Current_Animation->frames->h + ((*tmp)->collider.dimensions.second * 20) +80;

				App->render->DrawQuad(rect_bg, 255, 0, 0, 255); //background (red)
				App->render->DrawQuad(rect, 0, 255, 0, 255); //life (green)
			}
		}
		else if ((*tmp)->type >= Entity::entityType::SOLDIER)
		{
		App->render->Blit(entitiesTextures[(*tmp)->type], (*tmp)->position.first - (*tmp)->Current_Animation->frames->w*0.5, (*tmp)->position.second - (*tmp)->Current_Animation->frames->h*0.5, &((*tmp)->Current_Animation->GetCurrentFrame(dt)));

		//--- Draw Life Bar
		if ((*tmp)->health < (*tmp)->health_lv[(*tmp)->level] && (*tmp)->health > 0)
		{
			SDL_Rect rect, rect_bg;

			rect_bg.w = 30;
			rect.w = rect_bg.w * (*tmp)->health / (*tmp)->health_lv[(*tmp)->level];

			rect_bg.h = rect.h = 5;
			//rect_bg.x = rect.x = (*tmp)->position.first - ((*tmp)->collider.dimensions.first * 20) +((*tmp)->Current_Animation->GetCurrentFrame(dt).w / 2) - (rect_bg.w / 1.5);
			rect_bg.x = rect.x = (*tmp)->position.first - rect_bg.w/2;
			rect_bg.y = rect.y = (*tmp)->position.second - (*tmp)->Current_Animation->frames->h + /*((*tmp)->collider.dimensions.second * 20)*/ - 10;

			App->render->DrawQuad(rect_bg, 255, 0, 0, 255); //background (red)
			App->render->DrawQuad(rect, 0, 255, 0, 255); //life (green)
		}
}
		else
		{
 			App->render->Blit(entitiesTextures[(*tmp)->type], (*tmp)->position.first/*-((*tmp)->collider.dimensions.first*29)*/, (*tmp)->position.second - (*tmp)->Current_Animation->frames->h + ((*tmp)->collider.dimensions.second*20), &((*tmp)->Current_Animation->GetCurrentFrame(dt)));
		
			//--- Draw Life Bar
			if ((*tmp)->health < (*tmp)->health_lv[(*tmp)->level] && (*tmp)->health > 0)
			{
				SDL_Rect rect, rect_bg;

				rect_bg.w = 30;
				rect.w = rect_bg.w * (*tmp)->health / (*tmp)->health_lv[(*tmp)->level];

				rect_bg.h = rect.h = 5;
				rect_bg.x = rect.x = (*tmp)->position.first + ((*tmp)->Current_Animation->GetCurrentFrame(dt).w / 2) - (rect_bg.w / 1.5);
				rect_bg.y = rect.y = (*tmp)->position.second - (*tmp)->Current_Animation->frames->h + ((*tmp)->collider.dimensions.second * 20) - 10;

				App->render->DrawQuad(rect_bg, 255, 0, 0, 255); //background (red)
				App->render->DrawQuad(rect, 0, 255, 0, 255); //life (green)
			}
		}
			

		

		tmp++;
	}


	return ret;
}
Entity* EntityManager::AddEntity(bool isPlayer1, Entity::entityType type, pair<int, int> position, Collider collider, Animation* animation)
{
	BROFILER_CATEGORY("EntityManager AddEntity", Profiler::Color::AliceBlue);

	Entity* tmp = nullptr;

	if (type > Entity::entityType::BARRACKS) //if troops
	{
		switch (type)
		{
		case Entity::entityType::SOLDIER:
			tmp = new Soldier(isPlayer1, position, collider);
			break;

		case Entity::entityType::ENGINEER:
			tmp = new Engineer(isPlayer1, position, collider);
			break;

		case Entity::entityType::TANKMAN:
			tmp = new Tankman(isPlayer1, position, collider);
			break;

		case Entity::entityType::INFILTRATOR:
			tmp = new Infiltrator(isPlayer1, position, collider);
			break;

		case Entity::entityType::WAR_HOUND:
			tmp = new Hound(isPlayer1, position, collider);
			break;

		default:
			break;
		}
		if (tmp)
		{
			LoadAnimations((Troop*)tmp);

			if (isPlayer1 == true)
			{
				App->player1->troops.push_back((Troop*)tmp);
				App->player1->entities.push_back((Troop*)tmp);
			}
			else {
				App->player2->troops.push_back((Troop*)tmp);
				App->player2->entities.push_back((Troop*)tmp);
			}
		
			entity_list.push_back(tmp); // add to main entity list
			OrderEntities();

		}
	}
	// if building
	else {
		switch (type)
		{
		case Entity::entityType::TOWNHALL:
			tmp = new Townhall(isPlayer1, position, collider);
			break;

		case Entity::entityType::MAIN_DEFENSE: // this is the actual sentrygun
			tmp = new MainDefense(isPlayer1, position, collider);
			App->audio->PlayFx(SENTRYGUN_BUILD);

			break;

		case Entity::entityType::COMMAND_CENTER:
			tmp = new CmdCenter(isPlayer1, position, collider);
			break;

		case Entity::entityType::DEFENSE_AOE:
			//tesla
			App->audio->PlayFx(TESLA_BUILD);
			tmp = new DefenseAoe(isPlayer1, position, collider);

			if(tmp->fromPlayer1 && tmp->building->Finished())
				App->audio->PlayFx(ALLIED_LASER_B);
			else if (tmp->building->Finished())
				App->audio->PlayFx(SOVIET_LASER_B);

			break;

		case Entity::entityType::DEFENSE_TARGET:
			App->audio->PlayFx(SENTRYGUN_BUILD);
			tmp = new DefenseTarget(isPlayer1, position, collider);
			break;

		case Entity::entityType::MINES:
			App->audio->PlayFx(MINE_BUILD);
			tmp = new Mines(isPlayer1, position, collider);

			if (tmp->fromPlayer1 && tmp->building->Finished())
				App->audio->PlayFx(ALLIED_MINE_B);
			else if (tmp->building->Finished())
				App->audio->PlayFx(SOVIET_MINE_B);

			break;

		case Entity::entityType::BARRACKS:
			App->audio->PlayFx(BARRACKS_BUILD);
			tmp = new Barracks(isPlayer1, position, collider);

			if (tmp->fromPlayer1 && tmp->building->Finished())
				App->audio->PlayFx(ALLIED_BARRACKS_B);
			else if (tmp->building->Finished())
				App->audio->PlayFx(SOVIET_BARRACKS_B);

			break;

		case Entity::entityType::WALLS:
			tmp = new Walls(isPlayer1, position, collider, animation);
			break;

		}
		if (tmp) {
			if (isPlayer1 == true)
			{
				App->player1->gold -= App->player1->CheckCost(tmp->type);
				App->player1->buildings.push_back((Building*)tmp);
				App->player1->buildings = OrderBuildings(App->player1->buildings, true);
				App->player1->entities.push_back((Building*)tmp);
				App->player1->UpdateWalkabilityMap(P1_BUILDING, collider);

			}
			else // Player 2 -------------------------------
			{
				App->player2->gold -= App->player2->CheckCost(tmp->type);
				App->player2->buildings.push_back((Building*)tmp);
				App->player2->buildings = OrderBuildings(App->player2->buildings, true);
				App->player2->entities.push_back((Building*)tmp);
				App->player2->UpdateWalkabilityMap(P2_BUILDING, collider);

			}
			entity_list.push_back(tmp); // add to main entity list
			OrderEntities();
		}
	}

	return tmp;
}

char* EntityManager::GetName(Entity::entityType type) {
	switch (type)
	{
	case Entity::entityType::TOWNHALL:
		return"Townhall";
		break;
	case Entity::entityType::MAIN_DEFENSE:
		return"sentrygun";
		break;
	case Entity::entityType::COMMAND_CENTER:
		return"CommandCenter";
		break;
	case Entity::entityType::WALLS:
		return"Walls";
		break;
	case Entity::entityType::DEFENSE_AOE:
		return"aoe";
		break;
	case Entity::entityType::DEFENSE_TARGET:
		return"Tesla";
		break;
	case Entity::entityType::MINES:
		return"GoldMine";
		break;
	case Entity::entityType::BARRACKS:
		return"Barracks";
		break;
	case Entity::entityType::SOLDIER:
		return"BasicSoldier";
		break;
	case Entity::entityType::TANKMAN:
		return"tankman";
		break;
	case Entity::entityType::INFILTRATOR:
		return"infiltrator";
		break;
	case Entity::entityType::ENGINEER:
		return"engineer";
		break;
	case Entity::entityType::WAR_HOUND:
		return"war_hound";
		break;
	default:
		break;
	}
}

void EntityManager::OrderEntities()
{
	BROFILER_CATEGORY("EntityManager OrderEntities", Profiler::Color::Blue);
	priority_queue<Entity*, vector<Entity*>, Sorting> ListOrder;

	for (list<Entity*>::iterator tmp = entity_list.begin(); tmp != entity_list.end(); tmp++) //push entity_list items to Ordered List
	{
		ListOrder.push(*tmp);
	}

	entity_list.clear(); //clear unordered list

	while (ListOrder.empty() == false) //push ordered list to entity_list
	{
		entity_list.push_front(ListOrder.top());
		ListOrder.pop();
	}
}

list<Building*> EntityManager::OrderBuildings(list<Building*> List, bool isPlayer1)
{
	list<Building*> ListOrder;
	ListOrder.push_back(List.front()); // push first element of List to OrderList

	bool found = false;

	for (list<Building*>::iterator tmp = List.begin(); tmp != List.end(); tmp++) // traverse entity list (unordered)
	{
		for (list<Building*>::iterator tmp2 = ListOrder.begin(); tmp2 != ListOrder.end(); tmp2++) // traverse Ordered List
		{
			if (isPlayer1)
			{
				if (GetDepth(*tmp) < GetDepth(*tmp2)) // if tmp is further than tmp2
				{
					if (FindInList2(ListOrder, (*tmp)->position, (*tmp)->fromPlayer1, (*tmp)->type) == false)
					{
						ListOrder.insert(tmp2, *tmp); // add tmp in front of tmp2
						found = true;
					}

					break;
				}
			}
			else
			{
				if (GetDepth(*tmp) > GetDepth(*tmp2)) // if tmp is further than tmp2
				{
					if (FindInList2(ListOrder, (*tmp)->position, (*tmp)->fromPlayer1, (*tmp)->type) == false)
					{
						ListOrder.insert(tmp2, *tmp); // add tmp in front of tmp2
						found = true;
					}

					break;
				}
			}
			
		}
		if (found == false) // if tmp is the closest
		{
			if (FindInList2(ListOrder, (*tmp)->position, (*tmp)->fromPlayer1, (*tmp)->type) == false)
			{
				ListOrder.push_back(*tmp); // push to last place	
			}
		}
		found = false;
	}

	return ListOrder;
}

int EntityManager::GetDepth(Entity* entity)
{
	BROFILER_CATEGORY("Get Depth", Profiler::Color::Blue);
	pair<int,int> postemp = App->map->WorldToMap(entity->position.first, entity->position.second); // get map coords

	return (postemp.first + postemp.second); // return depth
}

void EntityManager::LoadAnimations(Troop* troop) {

	Troop* sample = (troop->fromPlayer1) ? (Soldier*)&soldier_p1_sample : (Soldier*)&soldier_p2_sample;;

	Troop* sample2 = nullptr; 

	switch (troop->type)
	{
	case Entity::entityType::SOLDIER:
		sample = (troop->fromPlayer1) ? (Soldier*)&soldier_p2_sample : (Soldier*)&soldier_p1_sample;
		sample2 = (troop->fromPlayer1) ? (Soldier*)&soldier_p2_inmune : (Soldier*)&soldier_p2_inmune;
		break;
	case Entity::entityType::TANKMAN:
		sample = (troop->fromPlayer1) ? (Soldier*)&soldier_p2_sample : (Soldier*)&soldier_p1_sample;
		//sample = (troop->fromPlayer1) ? (Tankman*)&tankman_p2_sample : (Tankman*)&tankman_p2_sample;
		break;
	case Entity::entityType::INFILTRATOR:
		sample = (troop->fromPlayer1) ? (Infiltrator*)&infiltrator_p1_sample : (Infiltrator*)&infiltrator_p1_sample;
		sample2 = (troop->fromPlayer1) ? (Soldier*)&infiltrator_p1_inmune : (Soldier*)&infiltrator_p1_inmune;
		break;
	case Entity::entityType::ENGINEER:
		sample = (troop->fromPlayer1) ? (Engineer*)&engineer_p2_sample : (Engineer*)&engineer_p1_sample;
		break;
	case Entity::entityType::WAR_HOUND:
		sample = (troop->fromPlayer1) ? (Hound*)&hound_p2_sample : (Hound*)&hound_p1_sample;
		break;

	default:
		LOG("ERROR LOADING ANIMATIONS: ERROR TYPE [EntityManager::LoadAnimations(Troop* troop)]");
		break;
	}

	troop->moving = vector<Animation*>(TroopDir::MAX_DIR, nullptr);
	troop->shooting = vector<Animation*>(TroopDir::MAX_DIR, nullptr);

	troop->moving_inv = vector<Animation*>(TroopDir::MAX_DIR, nullptr);
	troop->shooting_inv = vector<Animation*>(TroopDir::MAX_DIR, nullptr);

	troop->idle = sample->idle;
	troop->idle->speed = 0;
	troop->idle->SetCurrentFrame(2);

	troop->idle_inv = sample2->idle;
	troop->idle_inv->speed = 0;
	troop->idle_inv->SetCurrentFrame(2);

	for (int i = NORTH; i < MAX_DIR; i++) {
		troop->moving[i] = sample->moving[i];
		troop->shooting[i] = sample->shooting[i];
		troop->moving[i]->speed = sample->moving[i]->speed;
		troop->shooting[i]->speed = sample->shooting[i]->speed;

		troop->moving_inv[i] = sample2->moving[i];
		troop->shooting_inv[i] = sample2->shooting[i];
		troop->moving_inv[i]->speed = sample2->moving[i]->speed;
		troop->shooting_inv[i]->speed = sample2->shooting[i]->speed;
	}
	troop->Current_Animation = troop->moving[NORTH];

}


Entity* EntityManager::findEntity(pair <int,int> pos,bool fromplayer1, int attackrange)
{
	Player* enemy = (!fromplayer1)?App->player1:App->player2;

	Entity* found = *enemy->entities.begin();
	int distance = 0;
	pair<int, int> map_pos = App->map->WorldToMap(found->position.first, found->position.second);
	Is_inRange(pos,distance,map_pos,attackrange);
	int min_dist = distance;

	for (list<Entity*>::iterator tmp = enemy->entities.begin(); tmp != enemy->entities.end(); tmp++) // traverse entity list (unordered)
	{
		map_pos = App->map->WorldToMap((*tmp)->position.first, (*tmp)->position.second);
		
		if (Is_inRange(pos,distance,map_pos,attackrange))
		{

			if (min_dist >= distance )
			{
				found = (*tmp);
				min_dist = distance;
			}
		}
		
	}
	if (min_dist <= attackrange)
	{
		return found;
	}

	return nullptr;
	
}

bool EntityManager::Is_inRange(pair<int, int> pos, int &distance, pair <int,int> position, int range) {

	//posicion entre dos entidades cualquiera
	//determina si esta en el rango

	pair <int, int> vector_distance = { position.first - pos.first, position.second - pos.second };
	distance = (int)(sqrt(pow(vector_distance.first, 2) + pow(vector_distance.second/2, 2)));

	return distance <= range;
}

//void EntityManager:: Remove_fromList(Entity* entity)
//{
//	RELEASE(entity);
//	entity_list.remove(entity);
//}
//
//void EntityManager::Erase_fromList(list<Entity*>::iterator tmp)
//{
//	
//	entity_list.erase(tmp);
//}

bool EntityManager::FindInList(list<Entity*> List, pair <int,int> pos, bool fromplayer1, Entity::entityType type)
{
	BROFILER_CATEGORY("FindInList", Profiler::Color::Blue);
	bool ret = false;
	for (list<Entity*>::iterator tmp = List.begin(); tmp != List.end(); tmp++) // traverse entity list (unordered)
	{
		if ((*tmp)->fromPlayer1 == fromplayer1 && (*tmp)->position == pos && (*tmp)->type == type)
		{
			ret = true;
			break;
		}
	}
	return ret;
}

bool EntityManager::FindInList2(list<Building*> List, pair <int, int> pos, bool fromplayer1, Entity::entityType type)
{
	bool ret = false;
	for (list<Building*>::iterator tmp = List.begin(); tmp != List.end(); tmp++) // traverse entity list (unordered)
	{
		if ((*tmp)->fromPlayer1 == fromplayer1 && (*tmp)->position == pos && (*tmp)->type == type)
		{
			ret = true;
			break;
		}
	}
	return ret;
}
