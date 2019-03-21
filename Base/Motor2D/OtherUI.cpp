#include "Gui.h"
#include "Scene.h"
#include "Render.h"
#include "UI_Element.h"
#include "OtherUI.h"

OtherUI::OtherUI()
{
}

OtherUI::~OtherUI()
{
}

OtherUI::OtherUI(UI_type Type, Action Action, pair<int,int> pos, pair<int,int> Size, UI_Element* Parent, bool Visible, Dragable Dragable)
{
	type = Type;
	position.first = pos.first;
	position.second = pos.second;
	globalpos = position;
	size = Size;
	collider = { position.first, position.second, size.first, size.second };

	parent = Parent;
	visible = Visible;
	dragable = Dragable;

	if (parent != nullptr)
	{
		parent->children.push_back(this);
	}

	state = IDLE;
	action = Action;
}
