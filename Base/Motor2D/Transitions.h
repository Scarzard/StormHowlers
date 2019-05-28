#ifndef __Transitions_H__
#define __Transitions_H__

#include "Module.h"
#include "Timer.h"
#include "SDL\include\SDL_rect.h"

enum ListOfMapNames;

class Transitions : public Module
{
public:

	Transitions();

	//Destructor
	virtual ~Transitions();

	//Called before render is avalible
	bool Awake(pugi::xml_node&);

	//Called before the first frame
	bool Start();

	//Called each loop iteration
	bool Update(float dt);

	//Change Map
	bool ChangeMap(int newMap, float time);

	//returns true if changing
	bool IsChanging() const;

	//Change Scene
	bool SwitchScene(Module* SceneIn, Module* SceneOut);

	//returns true if switching
	bool IsSwitching() const;

public:
	bool fading = false;
	bool ContinueGame = false;

private:
	enum fade_step
	{
		none,
		fade_to_black,
		fade_from_black
	} 
	current_step = fade_step::none;
	
	int nextMap;
	SDL_Rect screen;

	Module* to_enable = nullptr;
	Module* to_disable = nullptr;

	bool map = false;
	bool scene = false;
	bool intro = false;
	bool main_menu = false;
	bool main_menu_in = false;

};

#endif // __Transitions_H__
