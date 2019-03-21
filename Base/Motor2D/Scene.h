#ifndef __Scene_H__
#define __Scene_H__

#include "Module.h"
#include "UI_Element.h"

struct SDL_Texture;

enum BUILD
{
	BUILD_DEFENSE_AOE = 1,
	BUILD_DEFENSE_TARGET,
	BUILD_MINES,
	BUILD_BARRACKS
};

enum DEPLOY
{
	DEPLOY_SOLDIER = 1,
	DEPLOY_TANKMAN,
	DEPLOY_INFILTRATOR,
	DEPLOY_ENGINEER,
	DEPLOY_WAR_HOUND
};

enum CAST
{
	CAST_MISSILES = 1,
	CAST_2,
	CAST_3
};

struct GeneralUI
{
	bool upgrade, repair;
	string name;
	uint level, health, unique;
};

class Scene : public Module
{
public:
	enum CURRENT_UI
	{
		CURR_MAIN = 1,
		CURR_BUILD,
		CURR_DEPLOY,
		CURR_CAST,
		CURR_GENERAL
	};

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool Load(pugi::xml_node& data);
	bool Save(pugi::xml_node& data) const;

	bool Load_level(int map);
	void SpawnEnemies();
	void SpawnEntities();

public:

	vector<string*> map_names;
	float fade_time;

	uint currentMap;
	uint currentUI;

	bool pause;
	bool godmode;
	bool to_end;
	bool change;

	string current_track;
	string current_fx;


private:
	SDL_Texture* debug_tex;

	// Main UI
	UI_Element* Health_UI;
	UI_Element* Gold_UI;

	UI_Element* Main_UI;
	UI_Element* Build_icon;
	UI_Element* Deploy_icon;
	UI_Element* Cast_icon;

	UI_Element* Build_UI;
	UI_Element* Def_AOE_icon;
	UI_Element* Def_Target_icon;
	UI_Element* Mines_icon;
	UI_Element* Barracks_icon;

	UI_Element* Deploy_UI;
	UI_Element* Soldier_icon;
	UI_Element* Tankman_icon;
	UI_Element* Infiltrator_icon;
	UI_Element* Engineer_icon;
	UI_Element* War_hound_icon;

	UI_Element* Cast_UI;
	UI_Element* Missiles_icon;
	UI_Element* Cast2_icon;
	UI_Element* Cast3_icon;

	// Selected Building UI
	UI_Element* General_UI;
	UI_Element* Upgrade_icon;
	UI_Element* Repair_icon;
	UI_Element* Name_text;
	UI_Element* Level_text;
	UI_Element* Health_text;
	UI_Element* Damage_text; //only for defense buildings
	UI_Element* Prod_text; //only for townhall & mines
	UI_Element* Capacity_text; //only for barracks
};

#endif // __Scene_H__
