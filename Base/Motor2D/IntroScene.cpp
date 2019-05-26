#include "IntroScene.h"
#include "Window.h"
#include "Textures.h"
#include "Scene.h"
#include "Input.h"
#include "Audio.h"
#include "Audio.h"
#include "Transitions.h"
#include "MainMenu.h"


IntroScene::IntroScene()
{
}

IntroScene::~IntroScene()
{
}

bool IntroScene::Start()
{
	bool ret = true;

	//LOG("Start IntroScene");

	pugi::xml_document doc;

	//App->xml->LoadXML("logo_scene.xml", doc);

	//pugi::xml_node skip_node = doc.child("file").child("skip_img");
	//pugi::xml_node logo_node = doc.child("file").child("logo");

	//SDL_Rect skip_image = { skip_node.attribute("x").as_int(),skip_node.attribute("y").as_int(),skip_node.attribute("w").as_int(),skip_node.attribute("h").as_int() };

	/*UI_window = App->win->GetWindowSize();

	window = App->gui->UI_CreateWin({ 0,0 }, win_size.x, win_size.y, 1, false);

	iPoint margin = { logo_node.attribute("x_margin").as_int(),logo_node.attribute("y_margin").as_int() };

	skip = window->CreateImage({ win_size.x - skip_image.w - margin.x,win_size.y - skip_image.h - margin.y }, skip_image);*/

	App->render->background = { 255, 255, 255 };

	/*int logo_size = logo_node.attribute("size").as_int();
*/
	App->video_manager->PlayVideo("intro.ogv", { 0,0,win_size.first,win_size.second });

	return ret;
}

bool IntroScene::Update(float dt)
{
	bool ret = true;

	if (!App->video_manager->IsPlaying())
	{
		if (logo_played == true)
		{
			App->scenechange->SwitchScene(App->main_menu, this, 2); //Here fadetoblack
			//App->scene->ChangeScene((Scene*)App->scene->menu_scene);
		}
	/*	else
		{
			logo_played = true;
			App->video_manager->PlayVideo("intro.ogv", { 0,0,win_size.first,win_size.second });
		}*/
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		App->scenechange->ContinueGame = true;
		App->scenechange->SwitchScene(App->main_menu, this, 2); //Here fadetoblack
	}

	return ret;
}

bool IntroScene::CleanUp()
{
	bool ret = true;

	//LOG("Clean IntroScene");

	App->render->background = { 0, 0, 0 };

	/*if (App->scene->GetCurrentScene() != App->scene->logo_scene)
		App->gui->DeleteElement(window);

	App->audio->RestartAudio();*/

	return ret;
}
