#ifndef __Scene_H__
#define __Scene_H__

#include "Module.h"
#include "UI_Element.h"
#include <stdio.h>

struct SDL_Texture;

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

struct PlayerUI
{
	bool isPlayer1;
	uint currentUI;

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


class Scene : public Module
{
public:
	Scene();
	virtual ~Scene();

	bool Awake(pugi::xml_node&);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	bool Load(pugi::xml_node& data);
	bool Save(pugi::xml_node& data) const;

	bool Load_level(int map);
	void SpawnEnemies();
	void SpawnEntities();

	void DrawLiveBar(Player* player);
	void AlliedVictorious();
	void SovietVictorious();

	//void changeSize(float time, int maxsize);

public:
	vector<string*> map_names;
	float fade_time;

	uint currentMap;
	uchar* Wmap;

	bool pause;
	bool godmode;
	bool to_end;
	bool change;

	string current_track;
	string current_fx;

	

	//testing animation
	SDL_Texture* spritesheet123 = nullptr;

//timer elements
	UI_Element* ui_timer;

	Timer world_clock;
	Timer world_seconds;
	Timer size_timer;
	char current_time[20] = "TIME: 000";
	uint worldminutes=0;
	uint worldseconds=0;
	uint size_timer_count = 0;
	int increase_decresease = 0;
	bool increase_size = true;
	bool change_font_size = false;
	bool reference_active = false;
	bool pausetimer = false;
	


private:
	SDL_Texture* debug_tex;
	SDL_Texture* cursor_tex;

	pair<int, int>	camera_motion;
	pair<int, int>	last_motion;
	bool	start_motion = false;
};

#endif // __Scene_H__
