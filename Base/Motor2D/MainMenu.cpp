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
	menu_background = App->gui->AddUIElement(true, UI_Element::UI_type::TEXTURE, UI_Element::Action::NONE, { 0, 0 }, { App->win->width, App->win->height }, nullptr, true);
	menu_background->texture = App->tex->Load(menu_bg_file_name.data());
	menu_background->rect = { 0, 0, App->win->width, App->win->height };
	App->render->zoom = 1;

	new_game_button = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::NEW_GAME, { 1273, 432 }, { 371, 87 }, menu_background, true);
	new_game_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 155, 22 }, { 0, 0 }, new_game_button, true, { false, false });
	new_game_text->label = new_game_label;
	new_game_text->color = { 255,255,9,255 };

	//exit_button = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::EXIT, { 1273, 519 }, { 371, 87 }, menu_background, true);
	//exit_button = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::EXIT, { 1273, 616 }, { 371, 87 }, menu_background, true);
	exit_button = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::EXIT, { 1273, 706 }, { 371, 87 }, menu_background, true);
	exit_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 1273, 432 }, { 371, 87 }, new_game_button, true);
	

	//ui_timer = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 800 ,00 }, { 0,0 }, nullptr, true, { false, false }, "Timer: 0s");

	App->player1->currentUI = Player::CURRENT_UI::CURR_MAIN_MENU;
	App->player1->UpdateFocus(App->player1->currentUI);

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
		menu_background->visible = false;
	}

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

	////Clear UI elements
	//list<UI_Element*>::iterator item2 = App->player1->UI_elements.begin();
	//while (item2 != App->player1->UI_elements.end())
	//{
	//	(*item2)->children.clear();
	//	RELEASE(*item2);
	//	item2++;
	//}
	//App->player1->UI_elements.clear();

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
		App->scenechange->SwitchScene(App->scene, App->main_menu); //Here fadetoblack
		menu_background->visible = false;
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
		//ShellExecuteA(NULL, "open", "https://github.com/Scarzard/StormHowlers", NULL, NULL, SW_SHOWNORMAL);
		break;
	}
}