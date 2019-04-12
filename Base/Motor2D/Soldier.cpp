#include "Soldier.h"


Soldier::Soldier()
{
	
}

Soldier::Soldier(bool isPlayer1, pair<int, int> pos):Troop(Entity::entityType::SOLDIER,isPlayer1,pos)
{
	
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

	Troop::Update(dt);
	return true;
}

void Soldier::CleanUp() {
	Troop::CleanUp();

}