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
#include "MainMenu.h"

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
	allied_win_name = conf.child("allied").attribute("file").as_string("");
	soviet_win_name = conf.child("soviet").attribute("file").as_string("");
	
	return ret;
}

// Called before the first frame
bool Gui::Start()
{
	atlas = App->tex->Load(atlas_file_name.data());
	allied_win_tex = App->tex->Load(allied_win_name.data());
	soviet_win_tex = App->tex->Load(soviet_win_name.data());

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

	case UI_Element::UI_type::TEXTURE:
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
			else if ((*UI_elem)->type == UI_Element::UI_type::LABEL && (*UI_elem) == App->main_menu->new_game_text && App->main_menu->active) //label main menu
			{
				App->tex->UnLoad((*UI_elem)->texture);
				(*UI_elem)->texture = App->font->Print((*UI_elem)->label, (*UI_elem)->color, App->font->main_menu_font);
				App->font->CalcSize((*UI_elem)->label, (*UI_elem)->size.first, (*UI_elem)->size.second);

				App->render->Blit((*UI_elem)->texture, (*UI_elem)->globalpos.first, (*UI_elem)->globalpos.second, 0, SDL_FLIP_NONE, 0);
			}
			else if ((*UI_elem)->type == UI_Element::UI_type::LABEL) //text
			{
				App->tex->UnLoad((*UI_elem)->texture);
				(*UI_elem)->texture = App->font->Print((*UI_elem)->label, (*UI_elem)->color, App->font->default);
				App->font->CalcSize((*UI_elem)->label, (*UI_elem)->size.first, (*UI_elem)->size.second);

				App->render->Blit((*UI_elem)->texture, (*UI_elem)->globalpos.first, (*UI_elem)->globalpos.second, 0, SDL_FLIP_NONE, 0);
			}
			else if ((*UI_elem)->type == UI_Element::UI_type::TEXTURE) //text
			{
				SDL_Rect r = { 0,0,App->win->width,App->win->height };
				if ((*UI_elem) == App->main_menu->menu_background)
				{
					SDL_RenderCopy(App->render->renderer, App->main_menu->menu_background->texture, NULL, &r);
				}
				else if ((*UI_elem) == App->player1->Pause_UI || (*UI_elem) == App->player1->Abort_UI)
				{
					SDL_RenderCopy(App->render->renderer, App->player1->Pause_UI->texture, NULL, &r);
				}
				
				
				
			}
			else //rest of ui
			{
				App->render->Blit(GetAtlas(), (*UI_elem)->globalpos.first, (*UI_elem)->globalpos.second, &(*UI_elem)->rect, SDL_FLIP_NONE, 0);
			}
		}
		if (UI_Debug == true)
		{
			if ((*UI_elem)->visible == true)
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
			else if ((*UI_elem)->type == UI_Element::UI_type::TEXTURE) //text
			{
				SDL_Rect r = { 0,0,App->win->width,App->win->height };
				
				if ((*UI_elem) == App->player2->Pause_UI || (*UI_elem) == App->player2->Abort_UI)
				{
					SDL_RenderCopy(App->render->renderer, App->player2->Pause_UI->texture, NULL, &r);
				}


			}
			else //rest of ui
			{
				App->render->Blit(GetAtlas(), (*UI_elem)->globalpos.first, (*UI_elem)->globalpos.second, &(*UI_elem)->rect, SDL_FLIP_NONE, 0);
			}
		}
		if (UI_Debug == true)
		{
			if ((*UI_elem)->visible == true)
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
			if (App->player1->Build_icon == data)
				data->rect = { 0, 0, 80, 81 };  // RED
			else
				data->rect = { 0, 81, 80, 79 }; //BLUE

			break;

		case UI_Element::Action::ACT_GOTO_DEPLOY:
			if (App->player1->Deploy_icon == data)
				data->rect = { 80, 0, 80, 81 }; // RED
			else
				data->rect = { 80, 81, 82, 79 }; //BLUE

			break;

		case UI_Element::Action::ACT_GOTO_CAST:
			if (App->player1->Cast_icon == data)
				data->rect = { 159, 0, 78, 81 }; // RED
			else
				data->rect = { 162, 81, 75, 79 }; //BLUE
			break;


			//--------- Building buttons
		case UI_Element::Action::ACT_BUILD_AOE: //POWER PLANT
			data->rect = { 746, 81, 85, 81 };
			break;
		case UI_Element::Action::ACT_BUILD_TARGET://PRISM TOWER
			data->rect = { 662, 81, 85, 81 };
			break;
		case UI_Element::Action::ACT_BUILD_MINE: //ORE REFINERY
			data->rect = { 576, 81, 85, 81 };
			break;
		case UI_Element::Action::ACT_BUILD_BARRACKS: //CLONING VATS
			data->rect = { 492, 81, 85, 81 };
			break;

			//--------- Troop buttons
		case UI_Element::Action::ACT_DEPLOY_SOLDIER: //GI
			data->rect = { 662, 0, 85, 81 };
			break;
		case UI_Element::Action::ACT_DEPLOY_TANKMAN: //TESLA TROOPER
			data->rect = { 576, 0, 85, 81 };
			break;
		case UI_Element::Action::ACT_DEPLOY_INFILTRATOR: // SPY
			data->rect = { 832, 0, 85, 81 };
			break;
		case UI_Element::Action::ACT_DEPLOY_ENGINEER:// ENGINEER
			data->rect = { 492, 0, 85, 81 };
			break;
		case UI_Element::Action::ACT_DEPLOY_WARHOUND://ATTACK DOG
			data->rect = { 746, 0, 85, 81 };
			break;

			//Special abilities button (cast)
		case UI_Element::Action::ACT_CAST_TANKS: //RHINO TANK
			data->rect = { 492, 161, 85, 81 };
			break;
		case UI_Element::Action::ACT_CAST_INVULNERABILITY://INVULNERABLE
			data->rect = { 576, 161, 85, 81 };
			break;
		case UI_Element::Action::ACT_CAST_MISSILES://V3 ROCKETS
			data->rect = { 662, 161, 85, 81 };
			break;

			//Main_Menu
		case UI_Element::Action::NEW_GAME:
			data->rect = { 744, 954, 371, 87 };
			break;
		case UI_Element::Action::EXIT:
			data->rect = { 744, 954, 371, 87 };
			break;

		//Win/Lose
		case UI_Element::Action::REMATCH:
			data->rect = { 581, 769, 187, 37 };
			break;
		case UI_Element::Action::RETURN_MAINMENU:
			data->rect = { 581, 769, 187, 37 };
			break;

			//------ Pause Menu ------
		case UI_Element::Action::RESUME_PAUSE:
			if (App->player1->Resume_Button == data)
				data->rect = { 581, 769, 301,59 };
			else
				data->rect = {581, 830, 300, 60};
			break;
		case UI_Element::Action::SETTINGS_PAUSE:
			if (App->player1->Settings_Button == data)
				data->rect = { 581, 769, 301,59 };
			else
				data->rect = { 581, 830, 300, 60 };
			break;
		case UI_Element::Action::ABORT_PAUSE:
			if (App->player1->Abort_Button== data)
				data->rect = { 581, 769, 301,59 };
			else
				data->rect = { 581, 830, 300, 60 };
			break;

		case UI_Element::Action::SURRENDER:
			if (App->player1->Quit == data)
				data->rect = { 581, 769, 301,59 };
			else
				data->rect = { 581, 830, 300, 60 };
			break;
		case UI_Element::Action::RESTART:
			if (App->player1->Restart == data)
				data->rect = { 581, 769, 301,59 };
			else
				data->rect = { 581, 830, 300, 60 };
			break;


		case UI_Element::Action::MUSIC_VOLUME:
			if (App->player1->Music_Settings == data)
				data->rect = { 1517, 830, 291, 54 };
			else
				data->rect = { 1816, 830, 291, 54 };
			break;
		case UI_Element::Action::FX_VOLUME:
			if (App->player1->FX_Settings == data)
				data->rect = { 1517, 830, 291, 54 };
			else
				data->rect = { 1816, 830, 291, 54 };
			break;
		}
		break;

	case UI_Element::State::HOVER:
		switch (data->action)
		{
		case UI_Element::Action::ACT_GOTO_BUILD:
			if (App->player1->Build_icon == data)
				data->rect = { 237, 0, 81, 81 };
			else
				data->rect = { 237, 81, 81, 79 };
			break;

		case UI_Element::Action::ACT_GOTO_DEPLOY:
			if (App->player1->Deploy_icon == data)
				data->rect = { 318, 0, 78, 81 };
			else
				data->rect = { 318, 81, 78, 79 };
			break;

		case UI_Element::Action::ACT_GOTO_CAST:
			if (App->player1->Cast_icon == data)
				data->rect = { 396, 0, 80, 81 };
			else
				data->rect = { 396, 81, 80, 79 };
			break;

			//--------- Building buttons
		case UI_Element::Action::ACT_BUILD_AOE: //POWER PLANT
			data->rect = { 1671, 81, 85, 81 };
			break;
		case UI_Element::Action::ACT_BUILD_TARGET://PRISM TOWER
			data->rect = { 1587, 81, 85, 81 };
			break;
		case UI_Element::Action::ACT_BUILD_MINE: //ORE REFINERY
			data->rect = { 1501, 81, 85, 81 };
			break;
		case UI_Element::Action::ACT_BUILD_BARRACKS: //CLONING VATS
			data->rect = { 1417, 81, 85, 81 };
			break;

			//--------- Troop buttons
		case UI_Element::Action::ACT_DEPLOY_SOLDIER: //GI
			data->rect = { 1587, 0, 85, 81 };
			break;
		case UI_Element::Action::ACT_DEPLOY_TANKMAN: //TESLA TROOPER
			data->rect = { 1501, 0, 85, 81 };
			break;
		case UI_Element::Action::ACT_DEPLOY_INFILTRATOR: // SPY
			data->rect = { 1757, 0, 85, 81 };
			break;
		case UI_Element::Action::ACT_DEPLOY_ENGINEER:// ENGINEER
			data->rect = { 1417, 0, 85, 81 };
			break;
		case UI_Element::Action::ACT_DEPLOY_WARHOUND://ATTACK DOG
			data->rect = { 1671, 0, 85, 81 };
			break;


			//Special abilities button (cast)
		case UI_Element::Action::ACT_CAST_TANKS: //RHINO TANK
			data->rect = { 1417, 161, 85, 81 };
			break;
		case UI_Element::Action::ACT_CAST_INVULNERABILITY://INVULNERABLE
			data->rect = { 1501, 161, 85, 81 };
			break;
		case UI_Element::Action::ACT_CAST_MISSILES://V3 ROCKETS
			data->rect = { 1587, 161, 85, 81 };
			break;

			//Main_Menu
		case UI_Element::Action::NEW_GAME:
			data->rect = { 1116, 954, 371, 86 };
			break;
		case UI_Element::Action::EXIT:
			data->rect = { 1116, 954, 371, 86 };
			break;

			//Win/Lose
		case UI_Element::Action::REMATCH:
			data->rect = { 769, 769, 186, 37 };
			break;
		case UI_Element::Action::RETURN_MAINMENU:
			data->rect = { 769, 769, 186, 37 };

			//------ Pause Menu ------
		case UI_Element::Action::RESUME_PAUSE:
			if (App->player1->Resume_Button == data)
				data->rect = { 881, 769, 301,59 };
			else
				data->rect = { 881, 830, 300, 60 };
			break;
		case UI_Element::Action::SETTINGS_PAUSE:
			if (App->player1->Settings_Button == data)
				data->rect = { 881, 769, 301,59 };
			else
				data->rect = { 881, 830, 300, 60 };
			break;
		case UI_Element::Action::ABORT_PAUSE:
			if (App->player1->Abort_Button == data)
				data->rect = { 881, 769, 301,59 };
			else
				data->rect = { 881, 830, 300, 60 };
			break;

		case UI_Element::Action::SURRENDER:
			if (App->player1->Quit == data)
				data->rect = { 881, 769, 301,59 };
			else
				data->rect = { 881, 830, 300, 60 };
			break;
		case UI_Element::Action::RESTART:
			if (App->player1->Restart == data)
				data->rect = { 881, 769, 301,59 };
			else
				data->rect = { 881, 830, 300, 60 };
			break;

		case UI_Element::Action::MUSIC_VOLUME:
			if (App->player1->Music_Settings == data)
				data->rect = { 1517, 884, 291, 54 };
			else
				data->rect = { 1816, 884, 291, 54 };
			break;
		case UI_Element::Action::FX_VOLUME:
			if (App->player1->FX_Settings == data)
				data->rect = { 1517, 884, 291, 54 };
			else
				data->rect = { 1816, 884, 291, 54 };
			break;
		}
		break;

	case UI_Element::State::LOGIC:
		switch (data->action)
		{
		case UI_Element::Action::ACT_GOTO_BUILD:
			data->rect = { 237, 163, 81, 80 };
			break;

		case UI_Element::Action::ACT_GOTO_DEPLOY:
			data->rect = { 318, 163, 78, 80 };
			break;

		case UI_Element::Action::ACT_GOTO_CAST:
			data->rect = { 396, 163, 79, 80 };
			break;

			//--------- Building buttons
		case UI_Element::Action::ACT_BUILD_AOE: //POWER PLANT
			data->rect = { 746, 81, 85, 81 };
			break;
		case UI_Element::Action::ACT_BUILD_TARGET://PRISM TOWER
			data->rect = { 662, 81, 85, 81 };
			break;
		case UI_Element::Action::ACT_BUILD_MINE: //ORE REFINERY
			data->rect = { 576, 81, 85, 81 };
			break;
		case UI_Element::Action::ACT_BUILD_BARRACKS: //CLONING VATS
			data->rect = { 492, 81, 85, 8 };
			break;

			//--------- Troop buttons
		case UI_Element::Action::ACT_DEPLOY_SOLDIER: //GI
			data->rect = { 662, 0, 85, 81 };
			break;
		case UI_Element::Action::ACT_DEPLOY_TANKMAN: //TESLA TROOPER
			data->rect = { 576, 0, 85, 81 };
			break;
		case UI_Element::Action::ACT_DEPLOY_INFILTRATOR: // SPY
			data->rect = { 832, 0, 85, 81 };
			break;
		case UI_Element::Action::ACT_DEPLOY_ENGINEER:// ENGINEER
			data->rect = { 492, 0, 85, 81 };
			break;
		case UI_Element::Action::ACT_DEPLOY_WARHOUND://ATTACK DOG
			data->rect = { 746, 0, 85, 81 };
			break;

			//Special abilities button (cast)
		case UI_Element::Action::ACT_CAST_TANKS: //RHINO TANK
			data->rect = { 492, 161, 85, 81 };
			break;
		case UI_Element::Action::ACT_CAST_INVULNERABILITY://INVULNERABLE
			data->rect = { 576, 161, 85, 81 };
			break;
		case UI_Element::Action::ACT_CAST_MISSILES://V3 ROCKETS
			data->rect = { 662, 161, 85, 81 };
			break;

			//Main_Menu
		case UI_Element::Action::NEW_GAME:
			data->rect = { 1488, 954, 371, 86 };
			break;
		case UI_Element::Action::EXIT:
			data->rect = { 1488, 954, 371, 86 };
			break;

			//Win/Lose
		case UI_Element::Action::REMATCH:
			data->rect = { 954, 769, 187, 37 };
			break;
		case UI_Element::Action::RETURN_MAINMENU:
			data->rect = { 954, 769, 187, 37 };

			//------ Pause Menu ------
		case UI_Element::Action::RESUME_PAUSE:
			if(App->player1->Resume_Button == data)
				data->rect = { 1181, 769, 301,59 };
			else
				data->rect = { 1181, 830, 300, 60 };
			break;
		case UI_Element::Action::SETTINGS_PAUSE:
			if (App->player1->Settings_Button == data)
				data->rect = { 1181, 769, 301,59 };
			else
				data->rect = { 1181, 830, 300, 60 };
			break;
		case UI_Element::Action::ABORT_PAUSE:
			if (App->player1->Abort_Button == data)
				data->rect = { 1181, 769, 301,59 };
			else
				data->rect = { 1181, 830, 300, 60 };
			break;

		case UI_Element::Action::SURRENDER:
			if (App->player1->Quit == data)
				data->rect = { 1181, 769, 301,59 };
			else
				data->rect = { 1181, 830, 300, 60 };
			break;
		case UI_Element::Action::RESTART:
			if (App->player1->Restart == data)
				data->rect = { 1181, 769, 301,59 };
			else
				data->rect = { 1181, 830, 300, 60 };
			break;

		case UI_Element::Action::MUSIC_VOLUME:
			if (App->player1->Music_Settings == data)
				data->rect = { 1517, 884, 291, 54 };
			else
				data->rect = { 1816, 884, 291, 54 };
			break;
		case UI_Element::Action::FX_VOLUME:
			if (App->player1->FX_Settings == data)
				data->rect = { 1517, 884, 291, 54 };
			else
				data->rect = { 1816, 884, 291, 54 };
			break;
		}
		break;

	case UI_Element::State::LOCKED:
		switch (data->action)
		{
		case UI_Element::Action::ACT_GOTO_BUILD:
			data->rect = { 0, 163, 80, 80 };
			break;

		case UI_Element::Action::ACT_GOTO_DEPLOY:
			data->rect = { 80, 63, 82, 80 };
			break;

		case UI_Element::Action::ACT_GOTO_CAST:
			data->rect = { 162, 163, 75, 80 };
			break;

			//--------- Building buttons
		case UI_Element::Action::ACT_BUILD_AOE: //POWER PLANT
			data->rect = { 1090, 81, 85, 81 };
			break;
		case UI_Element::Action::ACT_BUILD_TARGET://PRISM TOWER
			data->rect = { 1005, 81, 85, 81 };
			break;

			//Main_Menu - Locked needs to be changed
		case UI_Element::Action::NEW_GAME:
			data->rect = { 410,80,39,40 };
			break;
		case UI_Element::Action::EXIT:
			data->rect = { 332,80,39,40 };
			break;

		case UI_Element::Action::ACT_BUILD_MINE: //ORE REFINERY
			data->rect = { 919, 81, 85, 81 };
			break;
		case UI_Element::Action::ACT_BUILD_BARRACKS: //CLONING VATS
			data->rect = { 835, 81, 85, 81 };
			break;

			//--------- Troop buttons
		case UI_Element::Action::ACT_DEPLOY_SOLDIER: //GI
			data->rect = { 1090, 0, 85, 81 };
			break;
		case UI_Element::Action::ACT_DEPLOY_TANKMAN: //TESLA TROOPER
			data->rect = { 1005, 0, 85, 81 };
			break;
		case UI_Element::Action::ACT_DEPLOY_INFILTRATOR: // SPY
			data->rect = { 1260, 0, 85, 81 };
			break;
		case UI_Element::Action::ACT_DEPLOY_ENGINEER:// ENGINEER
			data->rect = { 921, 0, 85, 81 };
			break;
		case UI_Element::Action::ACT_DEPLOY_WARHOUND://ATTACK DOG
			data->rect = { 1175, 0, 85, 81 };
			break;

			//Special abilities button (cast) MUST GET COORDS
		//case UI_Element::Action::ACT_CAST_TANKS: //RHINO TANK
		//	data->rect = {  };
		//	break;
		//case UI_Element::Action::ACT_CAST_INVULNERABILITY://INVULNERABLE
		//	data->rect = {  };
		//	break;
		//case UI_Element::Action::ACT_CAST_MISSILES://V3 ROCKETS
		//	data->rect = {  };
		//	break;

				//------ Pause Menu ------
		case UI_Element::Action::RESUME_PAUSE:
			data->rect = { 769, 769, 301,59 };
			break;
		case UI_Element::Action::SETTINGS_PAUSE:
			data->rect = { 769, 769, 301,59 };
			break;
		case UI_Element::Action::ABORT_PAUSE:
			data->rect = { 769, 769, 301,59 };
			break;
			

		}
		break;
	}
}

