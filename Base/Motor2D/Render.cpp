#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Window.h"
#include "Render.h"
#include "Map.h"
#include "EntityManager.h"
#include "Brofiler\Brofiler.h"
#include "MainMenu.h"
#include "Player.h"

#define VSYNC true

Render::Render() : Module()
{
	name.assign("renderer");
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 0;
}

// Destructor
Render::~Render()
{}

// Called before render is available
bool Render::Awake(pugi::xml_node& config)
{
	LOG("Create SDL rendering context");
	bool ret = true;
	// load flags
	Uint32 flags = SDL_RENDERER_ACCELERATED;

	if(config.child("vsync").attribute("value").as_bool(true) == true)
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
		LOG("Using vsync");
	}

	renderer = SDL_CreateRenderer(App->win->window, -1, flags);
	App->win->ModifyWindow();

	if(renderer == NULL)
	{
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		camera.w = App->win->screen_surface->w;
		camera.h = App->win->screen_surface->h;
		camera.x = 913;
		camera.y = -387;
		/*camera.x =0;
		camera.y = 0;*/
		//To define
		zoom = (float)camera.w / 1680.0f;
		App->win->zoom_scale = zoom*App->win->scale;
	}

	original_camera = camera;
	return ret;
}

// Called before the first frame
bool Render::Start()
{
	LOG("render start");
	SDL_RenderGetViewport(renderer, &viewport);
	//SDL_RenderSetLogicalSize(renderer, 1680, 1050);
	App->render->zoom =(float) viewport.w/1680.0f;

	// Easy solution to keep game intact
	//SDL_RenderSetLogicalSize(renderer, App->win->width, App->win->height);

	return true;
}
void Render::WindowResized() {

	//Getting the new viewport
	SDL_RenderGetViewport(renderer, &viewport);

	int new_width =  viewport.w - camera.w;
	int new_height = viewport.h - camera.h;

	App->win->width  += new_width;
	App->win->height += new_height;

	camera.w += new_width;
	camera.h += new_height;

	viewport.w = camera.w;
	viewport.h = camera.h;


	// Centering the camera
	camera.x = -(App->map->data.center_tile.first*zoom - camera.w/2);
	camera.y = -(App->map->data.center_tile.second*zoom - camera.h/2);

	if (App->main_menu->active) {
		zoom = (float)viewport.w / 1680.0f;
	}
	else {
		zoom = 0.77f *(float)viewport.w / 1680.0f;		
	}
	
	//Readjust all the necessary things
	if (App->player1->Main_UI != nullptr) {
		App->player1->Main_UI->globalpos.second = (App->win->height / App->render->zoom) - 163*zoom;
		App->player1->Info_UI->globalpos.second = (App->win->height / App->render->zoom) - (280)*zoom;
	}

}
// Called each loop iteration
bool Render::PreUpdate()
{
	BROFILER_CATEGORY("Render PreUpdate", Profiler::Color::Green);

	SDL_RenderClear(renderer);
	return true;
}

bool Render::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
		WindowResized();
	}
	return true;
}

bool Render::PostUpdate()
{
	BROFILER_CATEGORY("Render PostUpdate", Profiler::Color::Green);

	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.g, background.a);
	SDL_RenderPresent(renderer);
	return true;
}

// Called before quitting
bool Render::CleanUp()
{
	LOG("Destroying SDL render");
	//SDL_DestroyRenderer(renderer);
	return true;
}

// Load Game State
bool Render::Load(pugi::xml_node& data)
{
	bool ret = true;
	camera.x = data.child("camera").attribute("x").as_int();
	camera.y = data.child("camera").attribute("y").as_int();
	return ret;
}

// Save Game State
bool Render::Save(pugi::xml_node& data) const
{
	bool ret = true;
	pugi::xml_node cam = data.append_child("camera");

	cam.append_attribute("x") = camera.x;
	cam.append_attribute("y") = camera.y;
	return ret;
}

void Render::SetBackgroundColor(SDL_Color color)
{
	background = color;
}

void Render::SetViewPort(const SDL_Rect& rect)
{
	SDL_RenderSetViewport(renderer, &rect);
}

void Render::GetViewPort(SDL_Rect& rect)
{
	SDL_RenderGetViewport(renderer, &rect);
}

void Render::ResetViewPort()
{
	SDL_RenderSetViewport(renderer, &viewport);
}


pair<int,int> Render::ScreenToWorld(int x, int y) const
{
	pair<int,int> ret;
	int scale = App->win->GetScale();
	ret.first = (x - camera.x / scale)/zoom;
	ret.second = (y - camera.y / scale)/zoom;
	return ret;
}

pair<int, int> Render::WorldtoScreen(int x, int y) const
{
	pair<int, int> ret;
	int scale = App->win->GetScale();
	ret.first = (x + camera.x / scale);
	ret.second = (y + camera.y / scale);
	return ret;
}

pair<int, int> Render::WorldtoScreen(pair<int,int> pos) const
{
	pair<int, int> ret;
	int scale = App->win->GetScale();
	ret.first = (pos.first + camera.x / scale);
	ret.second = (pos.second + camera.y / scale);
	return ret;
}

// Blit to screen
bool Render::Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section, SDL_RendererFlip flip, float speed, double angle, int pivot_x, int pivot_y) const
{
	BROFILER_CATEGORY("Blit", Profiler::Color::GreenYellow);

	bool ret = true;
	// Scale must be always positive (changed from uint to int)
	//float scale = App->win->zoom_scale;
	int scale = App->win->GetScale();

	SDL_Rect rect;
	rect.x = (int)( ((camera.x * speed) + (int)((x * scale*zoom))) );
	rect.y = (int)( ((camera.y * speed) + (int)((y * scale*zoom))) );

	if(section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= (scale*zoom) ;
	rect.h *= (scale*zoom) ;

	SDL_Point* p = NULL;
	SDL_Point pivot;

	if(pivot_x != INT_MAX && pivot_y != INT_MAX)
	{
		pivot.x = pivot_x;
		pivot.y = pivot_y;
		p = &pivot;
	}

	if(SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, flip) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::BlitEx(SDL_Texture* texture, int x, int y, const SDL_Rect* section, const SDL_Rect* new_rect, SDL_RendererFlip flip, float speed, double angle, int pivot_x, int pivot_y) const
{
	BROFILER_CATEGORY("Blit", Profiler::Color::GreenYellow);

	bool ret = true;
	// Scale must be always positive (changed from uint to int)
	//float scale = App->win->zoom_scale;
	int scale = App->win->GetScale();

	SDL_Rect rect;
	rect.x = (int)(((camera.x * speed) + (int)((x * scale*zoom))));
	rect.y = (int)(((camera.y * speed) + (int)((y * scale*zoom))));

	if (new_rect != NULL)
	{
		rect.w = new_rect->w;
		rect.h = new_rect->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= (scale*zoom);
	rect.h *= (scale*zoom);

	SDL_Point* p = NULL;
	SDL_Point pivot;

	if (pivot_x != INT_MAX && pivot_y != INT_MAX)
	{
		pivot.x = pivot_x;
		pivot.y = pivot_y;
		p = &pivot;
	}

	if (SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, flip) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled, bool use_camera) const
{
	bool ret = true;
	int scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);
	if(use_camera)
	{
		rec.x = (int)(((camera.x) + (int)((rect.x * scale)*zoom)));
		rec.y = (int)(((camera.y) + (int)((rect.y * scale)*zoom)));
	}

	rec.w *= (scale * zoom) + 0.1f;
	rec.h *= (scale * zoom) + 0.1f;

	int result = (filled) ? SDL_RenderFillRect(renderer, &rec) : SDL_RenderDrawRect(renderer, &rec);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;

	if(use_camera)
		result = SDL_RenderDrawLine(renderer, camera.x + x1 * scale, camera.y + y1 * scale, camera.x + x2 * scale, camera.y + y2 * scale);
	else
		result = SDL_RenderDrawLine(renderer, x1 * scale, y1 * scale, x2 * scale, y2 * scale);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawCircle(int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;
	SDL_Point points[360];

	float factor = (float)M_PI / 180.0f;

	for(uint i = 0; i < 360; ++i)
	{
		points[i].x = (int)(x + radius * cos(i * factor));
		points[i].y = (int)(y + radius * sin(i * factor));
	}

	result = SDL_RenderDrawPoints(renderer, points, 360);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}