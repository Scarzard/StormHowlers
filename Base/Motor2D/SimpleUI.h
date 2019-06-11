#ifndef __SIMPLEUI_H__
#define __SIMPLEUI_H__

#include "UI_Element.h"

class SimpleUI : public UI_Element
{
public:
	SimpleUI();
	SimpleUI(UI_type type, pair<int,int> position, pair<int,int> size, UI_Element* parent, bool visible, Dragable dragable, const char* label = nullptr);
	~SimpleUI();

};

#endif // __JUSTSIMPLEUI_H__