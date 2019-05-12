#ifndef __UI_ELEMENT_H__
#define __UI_ELEMENT_H__

#include "Module.h"
#include "App.h"
#include "Textures.h"
#include "Input.h"
#include "SDL\include\SDL_rect.h"

struct Dragable
{
	bool x, y;
};

class UI_Element
{
public:
	enum UI_type
	{
		UNKNOWN = 0,
		LABEL,
		IMAGE,
		PUSHBUTTON,
		SLIDER,
		WINDOW,
		TEXTURE
	};

	enum Action
	{
		NONE = 0,
		ACT_GOTO_MAIN,
		ACT_GOTO_BUILD,
		ACT_GOTO_DEPLOY,
		ACT_GOTO_CAST,

		ACT_BUILD_AOE,
		ACT_BUILD_TARGET,
		ACT_BUILD_MINE,
		ACT_BUILD_BARRACKS,

		ACT_DEPLOY_SOLDIER,
		ACT_DEPLOY_TANKMAN,
		ACT_DEPLOY_INFILTRATOR,
		ACT_DEPLOY_ENGINEER,
		ACT_DEPLOY_WARHOUND,

		ACT_CAST_TANKS,
		ACT_CAST_INVULNERABILITY,
		ACT_CAST_MISSILES,

		ACT_UPGRADE,
		ACT_REPAIR,

		//MainMenu Buttons
		NEW_GAME,
		SETTINGS,
		GO_BACK_TO_MM,
		CREDITS,
		LICENSE,
		AUTHORS,
		WEBSITE,
		EXIT,

		//Pause 
		RESUME_PAUSE,
		SETTINGS_PAUSE,
		ABORT_PAUSE,
		MUSIC_VOLUME,
		FX_VOLUME,
		SURRENDER,
		RESTART

		
	};

	enum State
	{
		NOTHING = 0,
		IDLE,
		HOVER,
		LOGIC,
		LOCKED,
		DRAG
	};

public:
	// Constructor
	UI_Element() {};
	UI_Element(UI_type type, UI_Element* parent, bool visible, Dragable dragable) {};

	// Destructor
	~UI_Element() {};

	virtual void DoLogic(Action action) {};

	void Center(bool x, bool y)
	{
		if (x)
		{
			position.first = parent->size.first / 2 - size.first / 2;
		}
		if (y)
		{
			position.second = parent->size.second / 2 - size.second / 2;
		}
	}

	void SetDragable(bool x, bool y)
	{
		if (x)
		{
			dragable.x = true;
		}
		if (y)
		{
			dragable.y = true;
		}
	}

	void Drag()
	{
		int x, y;
		App->input->GetMousePosition(x, y);
		
		if (dragable.x == true)
		{
			globalpos.first = x - click_pos.first + start_drag_pos.first;
		}
		if (dragable.y == true)
		{
			globalpos.second = y - click_pos.second + start_drag_pos.second;
		}
	}

public:
	SDL_Texture* texture;
	SDL_Rect rect;
	SDL_Color color;

	UI_type type;
	Action action;
	State state;

	Dragable dragable;
	pair<int,int> position;
	pair<int,int> globalpos;
	pair<int,int> start_drag_pos;
	pair<int,int> size;
	SDL_Rect collider;
	const char* label = nullptr;
	bool visible = true;
	bool locked = false;
	bool dragging = false;

	UI_Element* parent;
	list<UI_Element*> children;

	pair<int,int> click_pos;
};

#endif // __UI_ELEMENT__