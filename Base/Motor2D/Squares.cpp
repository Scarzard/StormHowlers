#include "Render.h"
#include "Squares.h"
#include "Scene.h"
#include "MainMenu.h"
#include "IntroScene.h"
#include "App.h"
#include "Window.h"
#include "Transitions.h"
#include "Log.h"

#include <random>
#include <algorithm>

Squares::Squares(int transition, j1Color color, float time) : j1Transitions(time)
{
	this->color = color;
	this->transition = transition;

	App->win->GetWindowSize(w, h);
	screen = { -(int)w, 0, (int)w, (int)h };
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);

	// Width and height of the squares
	square_w = w / number + 1;
	square_h = h / number + 1;

	for (int i = 0; i < number; i++) {
		for (int j = 0; j < number; j++) {
			squares_vec.push_back({ (int)(i * square_w), (int)(j * square_h) });
		}
	}

	// Randomizer of order in the list
	if (transition == 3 || transition == 4) {
		auto random = std::default_random_engine{};
		std::shuffle(std::begin(squares_vec), std::end(squares_vec), random);
	}
}

Squares::~Squares()
{}

void Squares::Start()
{

	j1Transitions::Start();

	// Squares appear gradually per columns
	if (transition == 1 || transition == 3) {
		int appear = (int)Interpolation(0, number*number, percentage);
		Draw(appear);
		//App->main_menu->menu_background->visible = false;
	}

	// Squares appear gradually per columns, width and height
	else if (transition == 2 || transition == 4) {
		int appear = (int)Interpolation(0, number*number, percentage);
		int width = (int)Interpolation(0, square_w, percentage);
		int height = (int)Interpolation(0, square_h, percentage);
		Draw(appear, width, height);
	}
}

void Squares::Change() 
{
	screen.x = screen.y = 0;
	SDL_SetRenderDrawColor(App->render->renderer, color.r, color.g, color.b, 255);
	SDL_RenderFillRect(App->render->renderer, &screen);

	if (App->scene->active)
		App->scenechange->SwitchScene(App->main_menu, App->scene);
	else if(App->main_menu->active)
		App->scenechange->SwitchScene(App->scene, App->main_menu);
	else if (App->intro->active)
		App->scenechange->SwitchScene(App->main_menu, App->intro);

	j1Transitions::Change();
}

void Squares::Exit() 
{

	j1Transitions::Exit();

	if (transition == 1 || transition == 3) {
		int appear = (int)Interpolation(number*number, 0, percentage);
		Draw(appear);
		
	}

	else if (transition == 2 || transition == 4) {
		int appear = (int)Interpolation(number*number, 0, percentage);
		int width = (int)Interpolation(square_w, 0, percentage);
		int height = (int)Interpolation(square_h, 0, percentage);
		Draw(appear, width, height);
	}
}

void Squares::Draw(int appear, int width, int height)
{

	SDL_Rect* Square_r = new SDL_Rect[appear];

	for (int i = 0; i < appear; i++) {
		Square_r[i].x = squares_vec[i].first;
		Square_r[i].y = squares_vec[i].second;

		if (height == 0)
			Square_r[i].h = square_h;
		else
			Square_r[i].h = height;

		if (width == 0)
			Square_r[i].w = square_w;
		else
			Square_r[i].w = width;
	}

	SDL_SetRenderDrawColor(App->render->renderer, color.r, color.g, color.b, 255);
	SDL_RenderFillRects(App->render->renderer, Square_r, appear);
}