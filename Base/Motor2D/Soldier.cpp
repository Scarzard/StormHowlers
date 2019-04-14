#include "Soldier.h"


Soldier::Soldier()
{
	
}

Soldier::Soldier(bool isPlayer1, pair<int, int> pos):Troop(Entity::entityType::SOLDIER,isPlayer1,pos)
{
	string path = "animation/" + name + ".tmx";
	LoadAnimations(isPlayer1, path.data());
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

void Soldier::LoadAnimations(bool isPlayer1, string path)
{
}
