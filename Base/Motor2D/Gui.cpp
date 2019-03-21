#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Render.h"
#include "Textures.h"
#include "Fonts.h"
#include "Input.h"
#include "Gui.h"
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
	list<UI_Element*>::reverse_iterator item = UI_elements.rbegin();
	while (item != UI_elements.rend())
	{
		if ((*item)->visible == true)
		{
			if (CheckMousePos(*item) == true && (*item)->dragging == false && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) != KEY_REPEAT) //hovering
			{
				(*item)->state = UI_Element::State::HOVER;
			}
			if (CheckClick(*item) == true && (*item)->state == UI_Element::State::HOVER) //on-click
			{
				if ((*item)->dragable.x == false && (*item)->dragable.y == false) //if not dragable
				{
					(*item)->state = UI_Element::State::LOGIC; //do logic
					if ((*item)->locked == true) //if locked
					{
						//App->audio->PlayFx(LOCKED);
					}
					else
					{
						//App->audio->PlayFx(CLICK);

						//--- Do logic
						if ((*item)->action == UI_Element::Action::ACT_GOTO_MAIN) { App->scene->currentUI = Scene::CURRENT_UI::CURR_MAIN; }
						else if ((*item)->action == UI_Element::Action::ACT_GOTO_BUILD) { App->scene->currentUI = Scene::CURRENT_UI::CURR_BUILD; }
						else if ((*item)->action == UI_Element::Action::ACT_GOTO_DEPLOY) { App->scene->currentUI = Scene::CURRENT_UI::CURR_DEPLOY; }
						else if ((*item)->action == UI_Element::Action::ACT_GOTO_CAST) { App->scene->currentUI = Scene::CURRENT_UI::CURR_CAST; }

						else if ((*item)->action == UI_Element::Action::ACT_BUILD_AOE)
						{
						}
						else if ((*item)->action == UI_Element::Action::ACT_BUILD_TARGET)
						{
						}
						else if ((*item)->action == UI_Element::Action::ACT_BUILD_MINE)
						{
						}
						else if ((*item)->action == UI_Element::Action::ACT_BUILD_BARRACKS)
						{
						}
						else if ((*item)->action == UI_Element::Action::ACT_BUILD_TARGET)
						{
						}
						else if ((*item)->action == UI_Element::Action::ACT_DEPLOY_SOLDIER)
						{
						}
						else if ((*item)->action == UI_Element::Action::ACT_DEPLOY_TANKMAN)
						{
						}
						else if ((*item)->action == UI_Element::Action::ACT_DEPLOY_INFILTRATOR)
						{
						}
						else if ((*item)->action == UI_Element::Action::ACT_DEPLOY_ENGINEER)
						{
						}
						else if ((*item)->action == UI_Element::Action::ACT_DEPLOY_WARHOUND)
						{
						}
						else if ((*item)->action == UI_Element::Action::ACT_CAST_MISSILES)
						{
						}
						else if ((*item)->action == UI_Element::Action::ACT_CAST_2)
						{
						}
						else if ((*item)->action == UI_Element::Action::ACT_CAST_3)
						{
						}
						else if ((*item)->action == UI_Element::Action::ACT_UPGRADE)
						{
						}
						else if ((*item)->action == UI_Element::Action::ACT_REPAIR)
						{
						}
					}
				}
				else //drag
				{
					(*item)->dragging = true;
					(*item)->Drag();

					////--- Do logic
					//if ((*item)->action == UI_Element::Action::ADJUST_VOL)
					//{
					//}

					////--- Check limits
					//if ((*item)->globalpos.first <= limit) //left limit
					//	(*item)->globalpos.first = limit;
					//else if ((*item)->globalpos.first >= limit) //right limit
					//	(*item)->globalpos.first = limit;

					UpdateChildren();
				}
			}
			else if (App->gui->CheckMousePos(*item) == false && (*item)->state != UI_Element::State::DRAG) //change to idle
			{
				(*item)->state = UI_Element::State::IDLE;
			}
		}
		UpdateState(*item);
		item++;
	}
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

	list<UI_Element*>::iterator item = UI_elements.begin();
	while (item != UI_elements.end())
	{
		(*item)->children.clear();
		RELEASE(*item);
		item++;
	}
	UI_elements.clear();

	return true;
}

// const getter for atlas
SDL_Texture* Gui::GetAtlas() const
{
	return atlas;
}

// class Gui ---------------------------------------------------

UI_Element* Gui::AddUIElement(UI_Element::UI_type type, UI_Element::Action action, pair<int,int> pos, pair<int,int> size, UI_Element* parent, bool visible, Dragable dragable, const char* label)
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

	if (UI_elem)
		UI_elements.push_back(UI_elem);

		return UI_elem;
}

bool Gui::Draw()
{
	bool ret = true;
	list<UI_Element*>::iterator UI_elem = UI_elements.begin();

	while (UI_elem != UI_elements.end())
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
	list<UI_Element*>::iterator item = UI_elements.begin();
	while (item != UI_elements.end())
	{
		if ((*item)->parent != nullptr)
		{
			if ((*item)->parent->visible == false)
			{
				(*item)->visible = false; //update visibility
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
				data->rect = {};
				break;
		}
		break;

	case UI_Element::State::HOVER:
		data->rect = {};
		break;

	case UI_Element::State::LOGIC:
		data->rect = {};
		break;
	}
}

