#include "MiniGameScene.h"
#include "Player.h"
#include "Render.h"


MiniGameScene::MiniGameScene()
{
}

MiniGameScene::~MiniGameScene()
{
	//rect = { App->render->camera.w / 2, App->render->camera.h / 2,50,50 };
	seconds_to_start = '3';
}

bool MiniGameScene::Start() {
	tex = App->tex->Load("animation/BasicSoldier_tiled.png");
	timer_label = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 110 , 27 }, { 0, 0 }, nullptr, true, { false, false });
	player1_label = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 110 , 500 }, { 0, 0 }, nullptr, true, { false, false });
	player2_label = App->gui->AddUIElement(true, UI_Element::UI_type::LABEL, UI_Element::Action::NONE, { 500 , 500 }, { 0, 0 }, nullptr, true, { false, false });
	player1_label->label = "0";
	player2_label->label = "0";
	timer_label->label = "3";
	timer_label->color = { 255,0,0,255 };
	player1_label->color = { 255,0,0,255 };
	player2_label->color = { 255,0,0,255 };

	return true;
}
bool MiniGameScene::Update(float dt) {
	int x, y;
	App->input->GetMousePosition(x, y);
	timer_label->position.first = x;
	timer_label->position.second = y;

	if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN) {
		player1_ready = true;
		player2_ready = true;
	}

	//App->render->Blit(tex, 0, 0, NULL);
	switch (mg_state)
	{
		case PREPARING:

			ShowSelectTroopsDialog();

			if (player1_ready && player2_ready) {
				mg_state = miniGameState::START;
				seconds_to_start = '3';
				timer.Start();
			}
		break;
		case miniGameState::START:

			App->render->Blit(tex, 0, 0, NULL);
			timer_label->label = &seconds_to_start;

			//3...2...1... YA
			if (timer.ReadSec() >= 1.0f) {
				seconds_to_start = '2';
				if (timer.ReadSec() >= 2.0f) {
					seconds_to_start = '1';
					if (timer.ReadSec() >= 3.0f) {
						seconds_to_start = '0';
						App->player1->DeployTroops(Entity::entityType::SOLDIER, 1, { 100 , 500 });
						App->player2->DeployTroops(Entity::entityType::SOLDIER, 1, { 500 , 500 });
						mg_state = miniGameState::PLAYING;						 
					}
				}
			}
			break;

		case miniGameState::PLAYING:
			if (App->player1->gamepad.Controller[BUTTON_X] == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN) {
				player1_counter++;
				sprintf_s(player1_ct_char,64,"%d",player1_counter);
				player1_label->label = player1_ct_char;
			}
			if (App->player2->gamepad.Controller[BUTTON_X] == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN) {
				player2_counter++;
				sprintf_s(player2_ct_char, 64, "%d", player2_counter);
				player2_label->label = player2_ct_char;
			}

			if (player1_counter >= goal_counter) {
				player1_won = true;
				mg_state = miniGameState::END;
			}
			if (player2_counter >= goal_counter) {
				player2_won = true;
				mg_state = miniGameState::END;
			}
			break;
		case miniGameState::END:
			//Switch scene
			break;

		default:
			break;
	}
	return true;
}
void MiniGameScene::ShowSelectTroopsDialog() {
	//App->render->DrawQuad(rect,  255, 0, 0, 1);
	//App->render->DrawQuad(&rect, 255, 0, 0, 1);
	//App->render->DrawQuad(&rect, 255, 0, 0, 1);
}

