#ifndef __Player_H__
#define __Player_H__

#include "Module.h"
#include "Troop.h"
#include "Building.h"
#include "Building.h"
#include "Troop.h"
#include "SDL\include\SDL_gamecontroller.h"

struct SDL_Texture;
struct GamePad
{
	SDL_GameController* GameController = nullptr;
	SDL_Joystick* joy = nullptr;

	bool Connected = false;
	bool State[MAX_BUTTONS];

	j1KeyState Controller[MAX_BUTTONS]; //This is the one you check. EXAMPLE: App->input->name.Controller[JOY_RIGHT] == KEY_REPEAT 

	int LeftAxisX = 0;
	int LeftAxisY = 0;

};

struct GeneralUI
{
	bool upgrade, repair;
	string name;
	uint level, health, unique;
};

struct Collider
{
	pair<int, int> dimensions;
	vector<pair<int,int>> tiles;
};


class Player : public Module
{
public:
	enum CURRENT_UI
	{
		NONE,
		CURR_MAIN_MENU,
		CURR_MAIN,
		CURR_BUILD,
		CURR_DEPLOY,
		CURR_CAST,
		CURR_GENERAL,
		CURR_PAUSE,
		CURR_PAUSE_SETTINGS,
		CURR_PAUSE_ABORT
	};

	Player();
	~Player();

	bool Start();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	void GetCurrentTile(int& x, int& y);

	void UpdateVisibility(); //update gui visibility
	void DoLogic(UI_Element* data); //gui actions

	void UpdateFocus(uint data);	//updates where the focus is pointing
	void GotoPrevWindows(uint data);
	UI_Element* GetUI_Element(uint data); //returns the window we are currently on

	bool CheckBuildingPos();
	void UpdateWalkabilityMap(bool isWalkable);
	bool DeleteEntity(Entity* entity);

public:
	bool onUI = false;
	bool isBuilding = false;
	bool isDeploying = false;
	bool isCasting = false;
	bool isPaused = false;

	Collider collider;
	pair<int, int> offset;
	Entity::entityType type;

	bool isPlayer1 = false;
	string team;

	uint gold = 0;
	uint time_iterator = 0;

	uint last_currentUI = 0;
	uint currentUI = 0;
	int actual_capacity = 0;


	SDL_Rect LiveBar;

	// Live of TOWN HALL
	int live = 0;
	Entity* TownHall = nullptr;

	GamePad gamepad;

	pair<int,int> currentTile;
	pair<int, int> x_limits_player1, y_limits_player1;
	pair<int, int> x_limits_player2, y_limits_player2;

	list<Building*> buildings;
	list<Troop*> troops;


	//---
	list<UI_Element*> UI_elements;


	list<UI_Element*>::iterator focus;
	list<UI_Element*>::iterator last_element;


	// --- UI --- //
	
	UI_Element* Gold_UI = nullptr;

	UI_Element* Main_UI = nullptr;
	UI_Element* Build_icon = nullptr;
	UI_Element* Deploy_icon = nullptr;
	UI_Element* Cast_icon = nullptr;

	UI_Element* Build_UI = nullptr;
	UI_Element* Def_AOE_icon = nullptr;
	UI_Element* Def_Target_icon = nullptr;
	UI_Element* Mines_icon = nullptr;
	UI_Element* Barracks_icon = nullptr;

	UI_Element* Deploy_UI = nullptr;
	UI_Element* Soldier_icon = nullptr;
	UI_Element* Tankman_icon = nullptr;
	UI_Element* Infiltrator_icon = nullptr;
	UI_Element* Engineer_icon = nullptr;
	UI_Element* War_hound_icon = nullptr;

	UI_Element* Cast_UI = nullptr;
	UI_Element* Missiles_icon = nullptr;
	UI_Element* Cast2_icon = nullptr;
	UI_Element* Cast3_icon = nullptr;

	// Selected Building UI
	UI_Element* General_UI = nullptr;
	UI_Element* Upgrade_icon = nullptr;
	UI_Element* Repair_icon = nullptr;
	UI_Element* Name_text = nullptr;
	UI_Element* Level_text = nullptr;
	UI_Element* Health_text = nullptr;
	UI_Element* UpgradeCost_text = nullptr;
	UI_Element* RepairCost_text = nullptr;
	UI_Element* Damage_text = nullptr; //only for defense buildings
	UI_Element* Prod_text = nullptr; //only for townhall & mines
	UI_Element* Capacity_text = nullptr; //only for barracks

	// Pause Menu
	UI_Element* Pause_UI = nullptr;

	UI_Element* Resume_Button = nullptr;
	UI_Element* Resume_text = nullptr;
	char resume_label[12] = "Resume Game";

	UI_Element* Settings_Button = nullptr;
	UI_Element* Settings_text = nullptr;
	char settings_label[9] = "Settings";

	UI_Element* Abort_Button = nullptr;
	UI_Element* Abort_text = nullptr;
	char abort_label[14] = "Abort Mission";

	// Pause Settings Menu
	UI_Element* Settings_UI = nullptr;

	UI_Element* Music_Settings = nullptr;
	UI_Element* Music_text = nullptr;
	char Music_label[14] = "Music Volume";
	UI_Element* Music_Slider = nullptr;
	UI_Element* Music_Slider_text = nullptr;
	char Music_Slider_label[5] = "0";
	UI_Element* Music_Slider_Button = nullptr;

	UI_Element* FX_Settings = nullptr;
	UI_Element* FX_text = nullptr;
	char FX_label[10] = "FX Volume";
	UI_Element* FX_Slider = nullptr;
	UI_Element* FX_Slider_text = nullptr;
	char FX_Slider_label[5] = "0";
	UI_Element* FX_Slider_Button = nullptr;

	// Pause Abort Mission

	UI_Element* Abort_UI = nullptr;
	UI_Element* Abort_Message = nullptr;
	char Abort_message_label[27] = "What would you like to do?";

	UI_Element* Quit = nullptr;
	UI_Element* Quit_text = nullptr;
	char Quit_label[10] = "Surrender";

	UI_Element* Restart = nullptr;
	UI_Element* Restart_text = nullptr;
	char Restart_label[13] = "Restart Game";
	

};

#endif // !__Player_H__