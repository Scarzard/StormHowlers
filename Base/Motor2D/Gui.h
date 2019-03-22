#ifndef __Gui_H__
#define __Gui_H__

#include "Module.h"
#include "UI_Element.h"
#include "Scene.h"
#include "SDL/include/SDL_rect.h"

#define CURSOR_WIDTH 2

class UI_Element;
// ---------------------------------------------------
class Gui : public Module
{
public:

	Gui();

	// Destructor
	virtual ~Gui();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool Draw();

	bool CheckMousePos(UI_Element* data); //(check joystick)
	bool CheckClick(UI_Element* data); //(check X)

	void UpdateChildren();
	void UpdateState(UI_Element* data);

	// Gui creation functions
	UI_Element* AddUIElement(UI_Element::UI_type Type, UI_Element::Action Action, pair<int,int> pos, pair<int,int> size, UI_Element* parent = nullptr, bool visible = false, Dragable dragable = { false, false }, const char* label = nullptr);

	SDL_Texture* GetAtlas() const;

private:

	string atlas_file_name;
	SDL_Texture* atlas;
	PlayerUI player;

public:

	list<UI_Element*> UI_elements;
	bool UI_Debug = false;

};

#endif //__J1_GUI_H__
