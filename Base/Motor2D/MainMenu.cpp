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
	settings_main_menu_name = conf.child("settings").attribute("file").as_string("");
	license_name = conf.child("license").attribute("file").as_string("");
	credits_name = conf.child("credits").attribute("file").as_string("");
	
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

	//PLAY
	new_game_button = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::NEW_GAME, { 1273, 432 }, { 371, 87 }, menu_background, true);
	new_game_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 155, 25 }, { 0, 0 }, new_game_button, true, { false, false });
	new_game_text->label = new_game_label;
	new_game_text->color = { 255, 255, 9, 255 };

	//EXIT
	exit_button = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::EXIT, { 1273, 710 }, { 371, 87 }, menu_background, true);
	exit_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 168, 25 }, { 0,0 }, exit_button, true, { false, false });
	exit_text->label = exit_label;
	exit_text->color = { 255, 255, 9, 255 };

	//CREDITS
	credits_button = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::CREDITS, { 1273, 617 }, { 371, 87 }, menu_background, true);
	credits_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 137, 25 }, { 0, 0 }, credits_button, true, { false, false });
	credits_text->label = credits_label;
	credits_text->color = { 255, 255, 9, 255 };
	
	//SETTINGS
	settings_button = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::SETTINGS, { 1273, 524 }, { 371, 87 }, menu_background, true);
	settings_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 125, 25 }, { 0, 0 }, settings_button, true, { false, false });
	settings_text->label = settings_label;
	settings_text->color = { 255, 255, 9, 255 };
	
	//SETTINGS WINDOW/BUTTONS
	MM_Settings_UI = App->gui->AddUIElement(true, UI_Element::UI_type::TEXTURE, UI_Element::Action::NONE, { 0, 0 }, { App->win->width - 400, App->win->height }, nullptr, false);
	MM_Settings_UI->texture = App->tex->Load(settings_main_menu_name.data());
	MM_Settings_UI->rect = { 0, 0, App->win->width, App->win->height };

	Music_Settings = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::MUSIC_VOLUME, { 100 ,100 }, { 301,59 }, MM_Settings_UI, false);
	Music_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 80, 18 }, { 0, 0 }, Music_Settings, false, { false, false });
	Music_text->label = Music_label;
	Music_text->color = { 255,255,9,255 };

	Settings_Title = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 1320, -90 }, { 0, 0 }, Music_Settings, false, { false, false });
	Settings_Title->label = Settings_Title_label;
	Settings_Title->color = { 255,255,9,255 };

	Music_Slider = App->gui->AddUIElement(true, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 50 , 100 }, { 651, 59 },  Music_Settings, false);
	Music_Slider->rect = { 1483, 770, 651, 59 };
	Music_Slider_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 640, 18 }, { 0, 0 },  Music_Slider, false, { false, false });
	Music_Slider_text->label =  Music_Slider_label;
	Music_Slider_text->color = { 255,255,9,255 };
	Music_Slider_Button = App->gui->AddUIElement(true, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 325 , 3 }, { 29, 55 },  Music_Slider, false);
	Music_Slider_Button->rect = { 2135, 773, 29, 55 };

	FX_Settings = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::FX_VOLUME, { 100 ,400 }, { 301,59 }, MM_Settings_UI, false);
	FX_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 100, 18 }, { 0, 0 },  FX_Settings, false, { false, false });
	FX_text->label =  FX_label;
	FX_text->color = { 255,255,9,255 };

	FX_Slider = App->gui->AddUIElement(true, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 50 , 100 }, { 651, 59 },  FX_Settings, false);
	FX_Slider->rect = { 1483, 770, 651, 59 };
	FX_Slider_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 640, 18 }, { 0, 0 },  FX_Slider, false, { false, false });
	FX_Slider_text->label =  FX_Slider_label;
	FX_Slider_text->color = { 255,255,9,255 };
	FX_Slider_Button = App->gui->AddUIElement(true, UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 325 , 3 }, { 29, 55 },  FX_Slider, false);
	FX_Slider_Button->rect = { 2135, 773, 29, 55 };


	//CREDITS WINDOW/BUTTONS
	Credits_UI = App->gui->AddUIElement(true, UI_Element::UI_type::TEXTURE, UI_Element::Action::NONE, { 0, 0 }, { App->win->width - 400, App->win->height }, nullptr, false);
	Credits_UI->texture = App->tex->Load(settings_main_menu_name.data());
	Credits_UI->rect = { 0, 0, App->win->width, App->win->height };

	website_button = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::WEBSITE, { 1273, 617 }, { 371, 87 }, Credits_UI, true);
	website_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 155, 25 }, { 0, 0 }, website_button, true, { false, false });
	website_text->label = website_label;
	website_text->color = { 255, 255, 9, 255 };

	//show authors
	view_authors_button = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::AUTHORS, { 1273, 524 }, { 371, 87 }, Credits_UI, true);
	view_authors_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 155, 25 }, { 0, 0 }, view_authors_button, true, { false, false });
	view_authors_text->label = view_authors_label;
	view_authors_text->color = { 255, 255, 9, 255 };

	//showlicense
	view_license_button = App->gui->AddUIElement(true, UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::LICENSE, { 1273, 432 }, { 371, 87 }, Credits_UI, true);
	view_license_text = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 155, 25 }, { 0, 0 }, view_license_button, true, { false, false });
	view_license_text->label = view_license_label;
	view_license_text->color = { 255, 255, 9, 255 };

	Credits_Title = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 150, -420 }, { 0, 0 }, view_license_button, false, { false, false });
	Credits_Title->label = Credits_Title_label;
	Credits_Title->color = { 255,255,9,255 };

	license_texture = App->gui->AddUIElement(true, UI_Element::UI_type::TEXTURE, UI_Element::Action::NONE, { 1000, 50 }, { 0, 0 }, nullptr, false);
	license_texture->texture = App->tex->Load(license_name.data());

	authors_texture = App->gui->AddUIElement(true, UI_Element::UI_type::TEXTURE, UI_Element::Action::NONE, { 50, 50 }, { 0, 0 }, nullptr, false);
	authors_texture->texture = App->tex->Load(credits_name.data());

	//ui_timer = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 800 ,00 }, { 0,0 }, nullptr, true, { false, false }, "Timer: 0s");

	App->player1->currentUI = Player::CURRENT_UI::CURR_MAIN_MENU;
	App->player1->UpdateFocus(App->player1->currentUI);

	string track = App->audio->folder_music + "/MainMenuSong.ogg";
	App->audio->PlayMusic(track.c_str());
	menu_background->visible = true;

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

	App->gui->Draw();	
	return true;
}

// Called each loop iteration
bool MainMenu::PostUpdate()
{
	BROFILER_CATEGORY("Main Menu PostUpdate", Profiler::Color::AliceBlue);
	
	if (Music_Slider_text->visible == true) //Music VOlume
	{
		sprintf_s(Music_Slider_label, "%u", App->audio->musicVolume);
	}
	
	if (FX_Slider_text->visible == true) //FX Volume
	{
		sprintf_s(FX_Slider_label, "%u", App->audio->sfxVolume);
	}
	
	if (Music_Slider_Button->visible == true) //MUSIC SLIDER POS
	{
		Music_Slider_Button->position.first = (550 * App->audio->musicVolume) / 100;
	}
	
	if (FX_Slider_Button->visible == true) //FX SLIDER POS
	{
		FX_Slider_Button->position.first = (550 * App->audio->sfxVolume) / 100;
	}

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
		App->player1->currentUI = Player::CURRENT_UI::CURR_MM_SETTINGS;
		App->player1->UpdateVisibility();
		App->player1->UpdateFocus(App->player1->currentUI);
		break;
	case::UI_Element::Action::CREDITS:
		App->player1->currentUI = Player::CURRENT_UI::CURR_MM_CREDITS;
		App->player1->UpdateVisibility();
		App->player1->UpdateFocus(App->player1->currentUI);
		break;
	case::UI_Element::Action::LICENSE:
		license_texture->visible = true;
		authors_texture->visible = false;
		break;
	case::UI_Element::Action::AUTHORS:
		license_texture->visible = false;
		authors_texture->visible = true;
		break;
	case::UI_Element::Action::WEBSITE:
		//Needs to be updated with game's website link, when it's done
		ShellExecuteA(NULL, "open", "https://github.com/stormhowlers/Command_and_Conquer_WarZone", NULL, NULL, SW_SHOWNORMAL);
		break;
	case::UI_Element::Action::EXIT:
		App->audio->PlayFx(EXIT);
		close_app = false;
		break;
	}
}