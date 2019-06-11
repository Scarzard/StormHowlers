#ifndef __OTHERUI_H__
#define __OTHERUI_H__

#include "UI_Element.h"

class OtherUI : public UI_Element
{
public:
	OtherUI();
	OtherUI(UI_type type, Action action, pair<int,int> position, pair<int,int> size, UI_Element* parent, bool visible, Dragable dragable);
	~OtherUI();

public:
	UI_Element* text;
	UI_Element* image;
};

#endif // __BUTTONS_H__
