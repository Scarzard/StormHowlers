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
	//settings_main_menu_name = conf.child("menu_bg").attribute("file").as_string("");
	
	// current_track = App->audio->tracks_path[1];
	return true;
}

// Called before the first frame
bool MainMenu::Start()
{
	//settings_texture = App->tex->Load(settings_main_menu_name.data());
	settings_texture = App->tex->Load("gui/mainmenu_settings_credits.png");

	menu_background = App->gui->AddUIElement(true, UI_Element::UI_type::TEXTURE, UI_Element::Action::NONE, { 0, 0 }, { App->win->width, App->win->height }, nullptr, true);
	menu_background->texture = App->tex->Load(menu_bg_file_name.data());
	menu_background->rect = { 0, 0, App->win->width, App->win->height };
	App->render->zoom = 1;
	//PLAY
	new_game_button = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::NEW_GAME, { 1273, 432 }, { 371, 87 }, menu_background, true);
	new_game_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 155, 25 }, { 0, 0 }, new_game_button, true, { false, false });
	new_game_text->label = new_game_label;
	new_game_text->color = { 255, 255, 9, 255 };
	//SETTINGS
	settings_button = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::SETTINGS, { 1273, 524 }, { 371, 87 }, menu_background, true);
	settings_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 155, 25 }, { 0, 0 }, settings_button, true, { false, false });
	settings_text->label = settings_label;
	settings_text->color = { 255, 255, 9, 255 };
	//CREDITS
	credits_button = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::CREDITS, { 1273, 618 }, { 371, 87 }, menu_background, true);
	credits_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 155, 25 }, { 0, 0 }, credits_button, true, { false, false });
	credits_text->label = credits_label;
	credits_text->color = { 255, 255, 9, 255 };
	
	//SETTINGS WINDOW/BUTTONS
	MM_Settings_UI = App->gui->AddUIElement(true, UI_Element::UI_type::TEXTURE, UI_Element::Action::NONE, { 0, 0 }, { App->win->width - 400, App->win->height }, nullptr, false);
	MM_Settings_UI->texture = settings_texture;
	MM_Settings_UI->rect = { 0, 0, App->win->width, App->win->height };

	Settings_Title = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 1425, 10 }, { 0, 0 }, MM_Settings_UI, false, { false, false });
	Settings_Title->label = Settings_Title_label;
	Settings_Title->color = { 255,255,9,255 };

	Music_Settings = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::MUSIC_VOLUME, { 100 ,100 }, { 301,59 }, MM_Settings_UI, false);
	Music_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 80, 18 }, { 0, 0 },  Music_Settings, false, { false, false });
	Music_text->label =  Music_label;
	Music_text->color = { 255,255,9,255 };

	Music_Slider = App->gui->AddUIElement(true, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 50 , 100 }, { 651, 59 },  Music_Settings, false);
	Music_Slider->rect = { 1483, 709, 651, 59 };
	Music_Slider_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 640, 18 }, { 0, 0 },  Music_Slider, false, { false, false });
	Music_Slider_text->label =  Music_Slider_label;
	Music_Slider_text->color = { 255,255,9,255 };
	Music_Slider_Button = App->gui->AddUIElement(true, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 325 , 3 }, { 29, 55 },  Music_Slider, false);
	Music_Slider_Button->rect = { 2135, 712, 29, 55 };

	FX_Settings = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::FX_VOLUME, { 100 ,400 }, { 301,59 }, MM_Settings_UI, false);
	FX_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 100, 18 }, { 0, 0 },  FX_Settings, false, { false, false });
	FX_text->label =  FX_label;
	FX_text->color = { 255,255,9,255 };

	FX_Slider = App->gui->AddUIElement(true, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 50 , 100 }, { 651, 59 },  FX_Settings, false);
	FX_Slider->rect = { 1483, 709, 651, 59 };
	FX_Slider_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 640, 18 }, { 0, 0 },  FX_Slider, false, { false, false });
	FX_Slider_text->label =  FX_Slider_label;
	FX_Slider_text->color = { 255,255,9,255 };
	FX_Slider_Button = App->gui->AddUIElement(true, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 325 , 3 }, { 29, 55 },  FX_Slider, false);
	FX_Slider_Button->rect = { 2135, 712, 29, 55 };
	//CREDITS WINDOW/BUTTONS
	Credits_UI = App->gui->AddUIElement(true, UI_Element::UI_type::TEXTURE, UI_Element::Action::NONE, { 0, 0 }, { App->win->width - 400, App->win->height }, nullptr, false);
	Credits_UI->texture = settings_texture; 
	Credits_UI->rect = { 0, 0, App->win->width, App->win->height };

	//EXIT
	exit_button = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::EXIT, { 1273, 711 }, { 371, 87 }, menu_background, true);
	exit_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 155, 25 }, { 0,0 }, exit_button, true, { false, false });
	exit_text->label = exit_label;
	exit_text->color = { 255, 255, 9, 255 };


	//ui_timer = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 800 ,00 }, { 0,0 }, nullptr, true, { false, false }, "Timer: 0s");

	App->player1->currentUI = Player::CURRENT_UI::CURR_MAIN_MENU;
	App->player1->UpdateFocus(App->player1->currentUI);

	string track = App->audio->folder_music + "/MainMenuSong.ogg";
	App->audio->PlayMusic(track.c_str());

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
		App->scenechange->SwitchScene(App->scene, App->main_menu,2);
		menu_background->visible = false;
	}

	//if (App->player1->currentUI = CURRENT_MM_UI::CURR_MM_SETTINGS || CURRENT_MM_UI::CURR_MM_SETTINGS && App->player1->gamepad.Controller[BUTTON_B])
	//{
	//	App->player1->UpdateFocus(App->player1->currentUI);
	//}
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

	/**/

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
		App->scenechange->SwitchScene(App->scene, App->main_menu,2); //Here fadetoblack
		App->audio->PlayFx(PLAY);
		break;

	case::UI_Element::Action::SETTINGS:
		App->player1->currentUI = CURRENT_MM_UI::CURR_MM_SETTINGS;
		App->player1->UpdateVisibility();
		break;

	case::UI_Element::Action::CREDITS:
		App->player1->currentUI = CURRENT_MM_UI::CURR_MM_CREDITS;
		App->player1->UpdateVisibility();
		break;

	case::UI_Element::Action::EXIT:
		App->audio->PlayFx(EXIT);
		close_app = false;
		break;

	//case::UI_Element::Action::WEBPAGE:
	//	//ShellExecuteA(NULL, "open", "https://github.com/Scarzard/StormHowlers", NULL, NULL, SW_SHOWNORMAL);
	//	break;
	}
}