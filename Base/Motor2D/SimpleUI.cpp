#include "UI_Element.h"
#include "SimpleUI.h"
#include "Fonts.h"

SimpleUI::SimpleUI()
{
}

SimpleUI::~SimpleUI()
{
}

SimpleUI::SimpleUI(UI_type Type, pair<int,int> pos, pair<int,int> Size, UI_Element* Parent, bool Visible, Dragable Dragable, const char* Label)
{
	type = Type;
	position.first = pos.first;
	position.second = pos.second;
	globalpos = position;
	size = Size;
	label = Label;

	parent = Parent;
	visible = Visible;
	dragable = Dragable;

	texture = App->font->Print(label, color);
	App->font->CalcSize(label, size.first, size.second);

	collider = { position.first, position.second, size.first, size.second };

	if (parent != nullptr)
	{
		parent->children.push_back(this);
	}

	state = IDLE;
}