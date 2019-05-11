#ifndef __j1MAIN_MENU_H__
#define __j1MAIN_MENU_H__

#include "Module.h"
#include "UI_Element.h"

struct SDL_Texture;

class MainMenu : public Module
{
public:

	MainMenu();

	// Destructor
	virtual ~MainMenu();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

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

	void UpdateVisibility(PlayerUI player);
	void DoLogic(UI_Element* data);


public:
	string menu_bg_file_name;

	UI_Element* menu_background = nullptr;

	UI_Element* new_game_button = nullptr;
	UI_Element* new_game_text = nullptr;
	char new_game_label[5] = "PLAY";

	UI_Element* settings_button = nullptr;
	UI_Element* settings_text = nullptr;
	char settings_label[9] = "SETTINGS";

	UI_Element* credits_button = nullptr;
	UI_Element* credits_text = nullptr;
	char credits_label[9] = "CREDITS";

	UI_Element* exit_button = nullptr;
	UI_Element* exit_text = nullptr;
	char exit_label[5] = "EXIT";

	string current_track;
	string current_fx;
private:
	bool close_app = true;
};

#endif // __j1MAIN_MENU_H__