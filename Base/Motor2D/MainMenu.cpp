#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Transitions.h"
#include "Scene.h"
#include "MainMenu.h"
#include "Gui.h"
#include "Fonts.h"
#include "UI_Element.h"
#include "Player.h"

#include <cstdio>
#include "Brofiler\Brofiler.h"

MainMenu::MainMenu() : Module()
{
	name.assign("main_menu");
}

// Destructor
MainMenu::~MainMenu()
{}

// Called before render is available
bool MainMenu::Awake(pugi::xml_node& conf)
{
	LOG("Loading Main Menu");

	menu_bg_file_name = conf.child("menu_bg").attribute("file").as_string("");
	// current_track = App->audio->tracks_path[1];
	return true;
}

// Called before the first frame
bool MainMenu::Start()
{
	menu_background = App->gui->AddUIElement(true, UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { 0, 0 }, { App->win->width, App->win->height }, nullptr, true);
	menu_background->texture = App->tex->Load(menu_bg_file_name);
	menu_background->rect = { 0, 0,  App->win->width, App->win->height };

	new_game_button = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::NEW_GAME, { 115, 2 }, { 39, 40 }, menu_background, true);

	exit_button = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::EXIT, { 0, 0 }, { 39, 40 }, menu_background, true);

	return true;
}

// Called each loop iteration
bool MainMenu::PreUpdate()
{
	BROFILER_CATEGORY("Main Menu PreUpdate", Profiler::Color::AliceBlue);

	return true;
}

// Called each loop iteration
bool MainMenu::Update(float dt)
{
	BROFILER_CATEGORY("Main Menu Update", Profiler::Color::AliceBlue);

	// -------
	if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)
	{
		App->gui->UI_Debug = !App->gui->UI_Debug;
	}
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		App->scenechange->ContinueGame = true;
		App->scenechange->SwitchScene(App->scene, App->main_menu);
	}

	App->render->Blit(menu_background->texture, 0, 0, &menu_background->rect, SDL_FLIP_NONE, 0);

	App->gui->Draw();
	
	return true;
}

// Called each loop iteration
bool MainMenu::PostUpdate()
{
	BROFILER_CATEGORY("Main Menu PostUpdate", Profiler::Color::AliceBlue);
	
	//--- Update GUI
	list<UI_Element*>::reverse_iterator item = App->player1->UI_elements.rbegin();
	while (item != App->player1->UI_elements.rend())
	{
		if ((*item)->visible == true)
		{
			if (((App->gui->CheckMousePos(*item) == true && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) != KEY_REPEAT) || (App->player1->CheckCursorPos(*item) == true && App->player1->gamepad.State[BUTTON_A] != KEY_REPEAT)) && (*item)->dragging == false) //hovering
			{
				(*item)->state = UI_Element::State::HOVER;
			}
			if (((App->gui->CheckClick(*item) == true && App->gui->CheckMousePos(*item) == true) || (App->player1->CheckCursorClick(*item) == true && App->player1->CheckCursorPos(*item) == true)) && (*item)->state == UI_Element::State::HOVER) //on-click
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
						if ((*item)->globalpos.second < App->win->height) 
						{
							DoLogic(*item);
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

					App->gui->UpdateChildren();
				}
			}
			else if (App->gui->CheckMousePos(*item) == false && App->player1->CheckCursorPos(*item) == false && (*item)->state != UI_Element::State::DRAG) //change to idle
			{
				(*item)->state = UI_Element::State::IDLE;
			}
		}
		App->gui->UpdateState(*item);
		item++;
	}


	return close_app;
}

// Called before quitting
bool MainMenu::CleanUp()
{
	LOG("Freeing Main Menu");

	App->tex->UnLoad(menu_background->texture);

	return true;
}

void MainMenu::UpdateVisibility(PlayerUI player) // Update GUI Visibility
{
	/*switch (player.currentUI)
	{
	case::Scene::CURRENT_UI::CURR_MAIN:

	}*/
	App->gui->UpdateChildren();
}

void MainMenu::DoLogic(UI_Element* data)
{
	switch (data->action)
	{
		//---- Main menu buttons
	case::UI_Element::Action::NEW_GAME:
		App->scenechange->ContinueGame = true;
		App->scenechange->SwitchScene(App->scene, App->main_menu);
		break;

	case::UI_Element::Action::CONTINUE:
		//
		break;

	case::UI_Element::Action::SETTINGS:
		//
		break;

	case::UI_Element::Action::EXIT:
		close_app = false;
		break;

	case::UI_Element::Action::WEBPAGE:
		ShellExecuteA(NULL, "open", "https://github.com/Scarzard/StormHowlers", NULL, NULL, SW_SHOWNORMAL);
		break;
	}
}