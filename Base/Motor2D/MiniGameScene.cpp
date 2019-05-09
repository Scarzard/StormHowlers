#include "MiniGameScene.h"
#include "Player.h"
#include "Render.h"
#include "Gui.h"


MiniGameScene::MiniGameScene()
{
}

MiniGameScene::~MiniGameScene()
{

}
bool MiniGameScene::Update(float dt) {

	switch (mg_state)
	{
	case PREPARING:
		ShowSelectTroopsDialog();

		if (player1_ready && player2_ready) {
			mg_state = miniGameState::READY;
		}
	break;
	case miniGameState::READY:
		break;
	case miniGameState::START:
		break;
	case miniGameState::PLAYING:
		break;
	case miniGameState::END:
		break;

	default:
		break;
	}
	return true;
}
void MiniGameScene::ShowSelectTroopsDialog() {
	App->render->DrawCircle(App->render->viewport.w / 2, App->render->viewport.h / 2, 25, 255, 0, 0, 1);
	App->render->DrawCircle(App->render->viewport.w / 2, App->render->viewport.h / 2, 50, 255, 0, 0, 1);
	App->render->DrawCircle(App->render->viewport.w / 2, App->render->viewport.h / 2, 75, 255, 0, 0, 1);
}
bool MiniGameScene::Start() {
	App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 9 , 9 }, { 0, 0 }, App->player1->Soldier_icon, false, { false, false },"SELECT YOUR TEAM");

	return true;
}
