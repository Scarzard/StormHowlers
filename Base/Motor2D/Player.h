#ifndef __Player_H__
#define __Player_H__

#include "Module.h"
#include "Troop.h"
#include "Building.h"
#include "Entity.h"
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

	bool JoystickState[4] = { false, false, false ,false };

};

struct GeneralUI
{
	bool upgrade, repair = false;
	string name;
	uint level, health, unique = 0;
};

class Player : public Module
{
public:
	enum DeployState {
		START,
		DEPLOYING,
		END
	};
	enum CURRENT_UI
	{
		NONE,
		CURR_MAIN_MENU,
		CURR_MM_SETTINGS,
		CURR_MM_CREDITS,
		CURR_SELECTING_BUILDING,
		CURR_MAIN,
		CURR_BUILD,
		CURR_DEPLOY,
		CURR_CAST,
		CURR_GENERAL,
		CURR_CREATE_TROOPS,
		CURR_CREATE_ABILITIES,
		CURR_PAUSE,
		CURR_PAUSE_SETTINGS,
		CURR_PAUSE_ABORT,
		CURR_WIN_SCREEN,
		ENDGAME, 
		DRAW
	};

	enum ABILITIES
	{
		INVULNERABLE,
		TANK,
		ROCKET
	};

	Player();
	Player(bool isplayer);
	~Player();

	bool Start();
	void RectangleSelection();
	bool DeployTroops(Entity::entityType type, int amount, pair<int, int> pos);
	bool Awake(pugi::xml_node &config);
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	void GetCurrentTile(int& x, int& y);

	// ---------------------- UI functions -----------------------------------------

	void UpdateVisibility(); //update gui visibility
	void DoLogic(UI_Element* data);
	void UpdateWalkabilityMap(char cell_type, Collider collider);
	//gui actions
	void UpdateFocus(uint data);	//updates where the focus is pointing
	void GotoPrevWindows(uint data);
	UI_Element* GetUI_Element(uint data); //returns the window we are currently on
	void Update_troop_image(int type);
	void UpdateGeneralUI(Entity* building);
	void Blit_Info();
	void ChangeTroopsState();

	// -----------------------------------------------------------------------------

	void CreateTroop(int type, int number);
	void CreateAbility(int type, int number);

	Building* GetSelectedBuilding() { return (*building_selected); };

	bool CheckBuildingPos();
	Collider GetCollider(pair<int, int> dimensions, pair<int,int> topTile_pos);
	//void UpdateWalkabilityMap(bool isWalkable, Collider collider);
	bool DeleteEntity(Entity* entity);

	int CheckCost(Entity::entityType type);
	int GoldKill(Entity* entity);

	void ChangeBuilding(int num);
	void ShowRange(Entity::entityType type, Collider collider);

private:
	int number = 0;
	
public:
	bool isBuilding = false;
	bool isDeploying = false;
	bool isCasting = false;
	bool isPaused = false;
	bool inmune = false;
	bool offensive = true;

	int timer_ref_sec = 0;
	int timer_ref_min = 0;
	int desired_second = 0;
	int desired_min = 0;

	bool pathfinding = true;



	Collider collider;
	pair<int, int> offset;
	Entity::entityType type;

	Entity* Townhall = nullptr;

	bool isPlayer1 = false;
	string team;

	int gold = 0;
	int gold_persecond = 0;

	SDL_Rect rectangle_origin = { 0,0,0,0 };

	bool gold_added = false;

	uint time_iterator = 0;

	uint last_currentUI = 0;
	uint currentUI = 0;

	DeployState deploy_state = DeployState::END;

	vector<SDL_Rect>* preview_rects;
	SDL_Texture* range_tex = nullptr;

	SDL_Rect LiveBar;
	int health, max_health = 0;

	//index for testing previews
	int curr = 1;
	int deploying_counter = 0;

	GamePad gamepad;

	pair<int,int> currentTile;
	pair<int, int> x_limits_player1, y_limits_player1;
	pair<int, int> x_limits_player2, y_limits_player2;

	list<Building*> buildings;
	SDL_Rect selected_texture = { 0,0,0,0 };
	list<Building*>::iterator building_selected;
	list<Building*>::iterator last_building;
	vector<Group*> groups = vector<Group*>();
	int group = 0;

	list<Troop*> troops;
	list<Entity*> entities;

	int SoldiersCreated = 0;
	int TankmansCreated = 0;
	int InfiltratorsCreated = 0;
	int EngineersCreated = 0;
	int WarHoundsCreated = 0;
	int BarracksCreated = 0;

	int Invulnerable_abilities = 0;
	int Rocket_abilities = 0;
	int Tank_abilities = 0;

	//---
	list<UI_Element*> UI_elements;

	list<UI_Element*>::iterator focus;
	list<UI_Element*>::iterator last_element;

	// --- UI --- //
	
	UI_Element* X_spawn = nullptr;
	UI_Element* X_text = nullptr;
	char X_label[10] = "Spawn All";
	UI_Element* A_spawn = nullptr;
	UI_Element* A_text = nullptr;
	char A_label[10] = "Spawn One";

	UI_Element* Gold_UI = nullptr;

	UI_Element* Y_to_Main = nullptr;
	UI_Element* Y_to_Main2 = nullptr;
	bool Y_pressed = false;

	UI_Element* SelectBuilding = nullptr;
	UI_Element* In_SelectBuilding = nullptr;

	UI_Element* RB_img = nullptr;
	UI_Element* LB_img = nullptr;
	

	UI_Element* Main_UI = nullptr;
	UI_Element* Build_icon = nullptr;
	UI_Element* Deploy_icon = nullptr;
	UI_Element* Cast_icon = nullptr;

	UI_Element* Build_UI = nullptr;
	UI_Element* Def_AOE_icon = nullptr;
	UI_Element* Def_Target_icon = nullptr;
	UI_Element* Mines_icon = nullptr;
	UI_Element* Barracks_icon = nullptr;
	UI_Element* Building_cost_text = nullptr;
	char Building_cost_label[10] = "0000 $";
	int BuildingCost = 0;

	UI_Element* Deploy_UI = nullptr;
	UI_Element* Soldier_icon = nullptr;
	UI_Element* Soldier_Off = nullptr;
	UI_Element* Soldier_Deff = nullptr;

	UI_Element* Tankman_icon = nullptr;
	UI_Element* Tankman_Off = nullptr;
	UI_Element* Tankman_Deff = nullptr;

	UI_Element* Infiltrator_icon = nullptr;
	UI_Element* Infiltrator_Off = nullptr;
	UI_Element* Infiltrator_Deff = nullptr;

	UI_Element* Engineer_icon = nullptr;
	UI_Element* Engineer_Off = nullptr;
	UI_Element* Engineer_Deff = nullptr;

	UI_Element* War_hound_icon = nullptr;
	UI_Element* WarHound_Off = nullptr;
	UI_Element* WarHound_Deff = nullptr;

	bool Soldier_Offensive = true;
	bool Tankman_Offensive = true;
	bool Engineer_Offensive = true;
	bool Infiltrator_Offensive = true;
	bool WarHound_Offensive = true;
	

	UI_Element* Troop_cost_text = nullptr;
	char Troop_cost_label[10] = "0000 $";
	int TroopCost = 0;

	UI_Element* Cast_UI = nullptr;
	UI_Element* Missiles_icon = nullptr;
	UI_Element* Missiles_text = nullptr;
	char missiles_label[4] = "0";
	UI_Element* Cast2_icon = nullptr;
	UI_Element* invulnerable_text = nullptr;
	char invulnerable_label[4] = "0";
	UI_Element* Cast3_icon = nullptr;
	UI_Element* tank_text = nullptr;
	char tank_label[4] = "0";

	// Selected Building UI
	UI_Element* General_UI = nullptr;
	UI_Element* Upgrade_icon = nullptr;
	UI_Element* Repair_icon = nullptr;
	UI_Element* UpgradeCost_text = nullptr;
	char upgrade_cost_label[20] = " - 0000 $";
	UI_Element* RepairCost_text = nullptr;
	char repair_cost_label[20] = " - 0000 $";
	UI_Element* Name_text = nullptr;
	char name_label[20] = "Name:  ";
	UI_Element* Level_text = nullptr;
	char level_label[20] = "Lvl:  ";
	UI_Element* Health_text = nullptr;
	char health_label[20] = "HP:  ";
	UI_Element* Damage_text = nullptr; //only for defense buildings
	char dmg_label[20] = "DMG:  ";
	UI_Element* Prod_text = nullptr; //only for townhall & mines
	char prod_label[20] = "$/s:  ";

	UI_Element* Capacity_text = nullptr; //only for barracks
	char capacity_label[20] = "$/s:  ";
	UI_Element* Create_troops = nullptr;
	UI_Element* Create_abilities = nullptr;

	// Create troops and abilities UI --------------------------
	UI_Element* Create_Troops_UI = nullptr;
	UI_Element* left_img = nullptr;
	UI_Element* troop_icon = nullptr;
	UI_Element* right_img= nullptr;
	UI_Element* plus = nullptr;
	UI_Element* minus = nullptr;
	UI_Element* accept_button = nullptr;
	UI_Element* cancel_button = nullptr;

	int UI_troop_type = 8; //select type of troop (9 is soldier) 
	int UI_ability_type = 0;
	int number_of_troops = 0;

	UI_Element* num_troops_text = nullptr;
	char num_troops_label[4] = "0";

	// ---------------------------------------------

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
	
	UI_Element* win_screen = nullptr;
	UI_Element* draw_screen = nullptr;
};

#endif // !__Player_H__