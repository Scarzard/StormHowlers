#include "Soldier.h"
#include "Render.h"


Soldier::Soldier()
{
	
}

Soldier::Soldier(bool isPlayer1, pair<int, int> pos):Entity(entityType::SOLDIER,isPlayer1,pos)
{
	tex = App->tex->Load("maps/meta.png");
	collider = { 64,0,64,32 };
}


Soldier::~Soldier()
{
}

bool Soldier::Update(float dt)
{
	int x = 0;
	int y = 0;
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) && position.first != 350) {

		App->input->GetMousePosition(x, y);
		position.first = x;
		position.second = y;
	}

	App->render->Blit(tex, position.first, position.second, &collider);

	return true;
}

