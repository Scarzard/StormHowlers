#include "IntroScene.h"
#include "Window.h"
#include "Textures.h"
#include "Scene.h"
#include "Input.h"
#include "Audio.h"
#include "Audio.h"
#include "j1TransitionManager.h"
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


	win_size.first = App->win->width;
	win_size.second = App->win->height;

	App->render->background = { 0, 0, 0 };

	int logo_size = 612;
	App->video_manager->PlayVideo("last.ogv", { 0,0,win_size.first,win_size.second });
	return ret;
}

bool IntroScene::Update(float dt)
{
	bool ret = true;
	if (finished == false)
	{
		if (!App->video_manager->IsPlaying())
		{
			finished = true;
			App->transitions->SquaresAppearing(1, Black, 2.0f);
		}
	}
	return ret;
}

bool IntroScene::CleanUp()
{
	bool ret = true;

	App->render->background = { 0, 0, 0 };

	return ret;
}
