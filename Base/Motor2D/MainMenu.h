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
	const char* menu_bg_file_name;

	UI_Element* menu_background;

	UI_Element* new_game_button;
	UI_Element* new_game_text;

	UI_Element* continue_button;
	UI_Element* continue_text;

	UI_Element* exit_button;
	UI_Element* exit_text;

	string current_track;
	string current_fx;
private:
	bool close_app = true;
};

#endif // __j1MAIN_MENU_H__