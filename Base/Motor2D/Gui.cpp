#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Render.h"
#include "Textures.h"
#include "Window.h"
#include "Fonts.h"
#include "Input.h"
#include "Gui.h"
#include "Player.h"
#include "UI_Element.h"
#include "SimpleUI.h"
#include "OtherUI.h"
#include "Scene.h"

Gui::Gui() : Module()
{
	name.assign("gui");
}

// Destructor
Gui::~Gui()
{}

// Called before render is available
bool Gui::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");
	bool ret = true;

	atlas_file_name = conf.child("atlas").attribute("file").as_string("");
	
	return ret;
}

// Called before the first frame
bool Gui::Start()
{
	atlas = App->tex->Load(atlas_file_name.data());
	return true;
}

// Update all guis
bool Gui::PreUpdate()
{
	return true;
}

// Called after all Updates
bool Gui::PostUpdate()
{
	UpdateChildren();
	return true;
}

// Called before quitting
bool Gui::CleanUp()
{
	LOG("Freeing GUI");

	App->tex->UnLoad(atlas);
	return true;
}

// const getter for atlas
SDL_Texture* Gui::GetAtlas() const
{
	return atlas;
}

// class Gui ---------------------------------------------------

UI_Element* Gui::AddUIElement(bool player1, UI_Element::UI_type type, UI_Element::Action action, pair<int, int> pos, pair<int, int> size, UI_Element* parent, bool visible, Dragable dragable, const char* label)
{
	UI_Element* UI_elem = nullptr;

	switch (type)
	{
	case UI_Element::UI_type::LABEL:
		UI_elem = new SimpleUI(type, pos, size, parent, visible, dragable);
		break;

	case UI_Element::UI_type::IMAGE:
		UI_elem = new OtherUI(type, action, pos, size, parent, visible, dragable);
		break;

	case UI_Element::UI_type::PUSHBUTTON:
		UI_elem = new OtherUI(type, action, pos, size, parent, visible, dragable);
		break;

	case UI_Element::UI_type::SLIDER:
		UI_elem = new OtherUI(type, action, pos, size, parent, visible, dragable);
		break;

	case UI_Element::UI_type::WINDOW:
		UI_elem = new OtherUI(type, action, pos, size, parent, visible, dragable);
		break;
	}

	if (UI_elem && player1 == true)
		App->player1->UI_elements.push_back(UI_elem);

	else if (UI_elem && player1 == false)
		App->player2->UI_elements.push_back(UI_elem);

	return UI_elem;
}

bool Gui::Draw()
{
	bool ret = true;

	//Player 1
	list<UI_Element*>::iterator UI_elem = App->player1->UI_elements.begin();
	while (UI_elem != App->player1->UI_elements.end())
	{
		if ((*UI_elem)->visible == true)
		{
			if ((*UI_elem)->type == UI_Element::UI_type::LABEL && (*UI_elem)==App->scene->ui_timer) //timer
			{
				App->tex->UnLoad((*UI_elem)->texture);
				(*UI_elem)->texture = App->font->Print((*UI_elem)->label, (*UI_elem)->color,App->font->actual_font);
				App->font->CalcSize((*UI_elem)->label, (*UI_elem)->size.first, (*UI_elem)->size.second);

				App->render->Blit((*UI_elem)->texture, (*UI_elem)->globalpos.first, (*UI_elem)->globalpos.second, 0, SDL_FLIP_NONE, 0);
			}
			else if ((*UI_elem)->type == UI_Element::UI_type::LABEL) //text
			{
				App->tex->UnLoad((*UI_elem)->texture);
				(*UI_elem)->texture = App->font->Print((*UI_elem)->label, (*UI_elem)->color);
				App->font->CalcSize((*UI_elem)->label, (*UI_elem)->size.first, (*UI_elem)->size.second);

				App->render->Blit((*UI_elem)->texture, (*UI_elem)->globalpos.first, (*UI_elem)->globalpos.second, 0, SDL_FLIP_NONE, 0);
			}
			else //rest of ui
			{
				App->render->Blit(GetAtlas(), (*UI_elem)->globalpos.first, (*UI_elem)->globalpos.second, &(*UI_elem)->rect, SDL_FLIP_NONE, 0);
			}
		}
		if (UI_Debug == true)
		{
			App->render->DrawQuad((*UI_elem)->collider, 255, 0, 0, 255, false, false);
		}
		UI_elem++;
	}

	//Player 2
	UI_elem = App->player2->UI_elements.begin();
	while (UI_elem != App->player2->UI_elements.end())
	{
		if ((*UI_elem)->visible == true)
		{
			if ((*UI_elem)->type == UI_Element::UI_type::LABEL) //text
			{
				App->tex->UnLoad((*UI_elem)->texture);
				(*UI_elem)->texture = App->font->Print((*UI_elem)->label, (*UI_elem)->color);
				App->font->CalcSize((*UI_elem)->label, (*UI_elem)->size.first, (*UI_elem)->size.second);

				App->render->Blit((*UI_elem)->texture, (*UI_elem)->globalpos.first, (*UI_elem)->globalpos.second, 0, SDL_FLIP_NONE, 0);
			}
			else //rest of ui
			{
				App->render->Blit(GetAtlas(), (*UI_elem)->globalpos.first, (*UI_elem)->globalpos.second, &(*UI_elem)->rect, SDL_FLIP_NONE, 0);
			}
		}
		if (UI_Debug == true)
		{
			App->render->DrawQuad((*UI_elem)->collider, 255, 0, 0, 255, false, false);
		}

		UI_elem++;
	}


	return ret;
}


bool Gui::CheckMousePos(UI_Element* data)
{
	bool ret = false;

	int x, y;
	App->input->GetMousePosition(x, y);
	SDL_Rect MouseCollider = { x,y,1,1 };
	if (SDL_HasIntersection(&MouseCollider, &data->collider))
	{
		ret = true;
	}

	return ret;
}

bool Gui::CheckClick(UI_Element* data)
{
	bool ret = false;

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		App->input->GetMousePosition(data->click_pos.first, data->click_pos.second);
		data->start_drag_pos = data->globalpos;

		ret = true;
	}
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		ret = true;
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
	{
		data->dragging = false;
		return false;
	}

	return ret;
}

void Gui::UpdateChildren()
{
	//Player 1
	list<UI_Element*>::iterator item = App->player1->UI_elements.begin();
	while (item != App->player1->UI_elements.end())
	{
		if ((*item)->parent != nullptr)
		{
			if ((*item)->parent->visible != (*item)->visible)
			{
				(*item)->visible = (*item)->parent->visible; //update visibility
			}
			(*item)->globalpos.first = (*item)->parent->globalpos.first + (*item)->position.first; //update position x
			(*item)->globalpos.second = (*item)->parent->globalpos.second + (*item)->position.second; //update position y
		}
		(*item)->collider = { (*item)->globalpos.first, (*item)->globalpos.second, (*item)->size.first, (*item)->size.second }; //update collider
		item++;
	}

	//Player 2
	item = App->player2->UI_elements.begin();
	while (item != App->player2->UI_elements.end())
	{
		if ((*item)->parent != nullptr)
		{
			if ((*item)->parent->visible != (*item)->visible)
			{
				(*item)->visible = (*item)->parent->visible; //update visibility
			}
			(*item)->globalpos.first = (*item)->parent->globalpos.first + (*item)->position.first; //update position x
			(*item)->globalpos.second = (*item)->parent->globalpos.second + (*item)->position.second; //update position y
		}
		(*item)->collider = { (*item)->globalpos.first, (*item)->globalpos.second, (*item)->size.first, (*item)->size.second }; //update collider
		item++;
	}

}

void Gui::UpdateState(UI_Element* data) //change sprites depending on current state
{
	switch (data->state) //(estaria bien poner los sprites en un xml o algo para hacer mas eficiente esta funcion)
	{
	case UI_Element::State::IDLE:
		switch (data->action)
		{
		case UI_Element::Action::ACT_GOTO_BUILD:
			data->rect = { 332,0,39,40 };
			break;

		case UI_Element::Action::ACT_GOTO_DEPLOY:
			data->rect = { 371,0,39,40 };
			break;

		case UI_Element::Action::ACT_GOTO_CAST:
			data->rect = { 410,0,39,40 };
			break;

		case UI_Element::Action::ACT_BUILD_AOE:
			data->rect = { 332,0,39,40 };

			//-----Needs to be changed
		case UI_Element::Action::NEW_GAME:
			data->rect = { 410,0,39,40 };
			break;
			//change values
		case UI_Element::Action::EXIT:
			data->rect = { 371,0,39,40 };
			break;
		}
		break;

	case UI_Element::State::HOVER:
		switch (data->action)
		{
		case UI_Element::Action::ACT_GOTO_BUILD:
			data->rect = { 449,0,39,40 };
			break;

		case UI_Element::Action::ACT_GOTO_DEPLOY:
			data->rect = { 488,0,39,40 };
			break;

		case UI_Element::Action::ACT_GOTO_CAST:
			data->rect = { 527,0,39,40 };
			break;

		case UI_Element::Action::ACT_BUILD_AOE:
			data->rect = { 449,0,39,40 };
        
			//-----Needs to be changed
		case UI_Element::Action::NEW_GAME:
			data->rect = { 527,0,39,40 };
			break;
			//change values
		case UI_Element::Action::EXIT:
			data->rect = { 488,0,39,40 };

			break;
		}
		break;

	case UI_Element::State::LOGIC:
		switch (data->action)
		{
		case UI_Element::Action::ACT_GOTO_BUILD:
			data->rect = { 449,80,39,40 };
			break;

		case UI_Element::Action::ACT_GOTO_DEPLOY:
			data->rect = { 488,80,39,40 };
			break;

		case UI_Element::Action::ACT_GOTO_CAST:
			data->rect = { 527,80,39,40 };
			break;

		case UI_Element::Action::ACT_BUILD_AOE:
			data->rect = { 449,80,39,40 };

			//-----Needs to be changed
		case UI_Element::Action::NEW_GAME:
			data->rect = { 527,80,39,40 };
			break;
		case UI_Element::Action::EXIT:
			data->rect = { 488,80,39,40 };
			break;
		}
		break;

	case UI_Element::State::LOCKED:
		switch (data->action)
		{
		case UI_Element::Action::ACT_GOTO_BUILD:
			data->rect = { 332,80,39,40 };
			break;

		case UI_Element::Action::ACT_GOTO_DEPLOY:
			data->rect = { 371,80,39,40 };
			break;

		case UI_Element::Action::ACT_GOTO_CAST:
			data->rect = { 410,80,39,40 };
			break;
			//-----Needs to be changed
		case UI_Element::Action::NEW_GAME:
			data->rect = { 410,80,39,40 };
			break;
		case UI_Element::Action::EXIT:
			data->rect = { 332,80,39,40 };
			break;
		}
		break;
	}
}

