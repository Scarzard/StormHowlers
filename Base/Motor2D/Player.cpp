#include "App.h"
#include "Player.h"
#include "Audio.h"
#include "Render.h"
#include "Map.h"
#include "Window.h"
#include "Textures.h"
#include "Input.h"
#include "Scene.h"
#include "Transitions.h"
#include "EntityManager.h"
#include "Log.h"
#include "Brofiler\Brofiler.h"


Player::Player() : Module()
{
}

Player::~Player()
{
}

bool Player::Awake(pugi::xml_node & config)
{
	LOG("Loading Player");
	bool ret = true;

	// Player starting point
	config = config.child("player");

	return ret;
}

bool Player::Start()
{
	return true;
}

bool Player::Update(float dt)
{
	BROFILER_CATEGORY("Player Update", Profiler::Color::Black);

	return true;
}

bool Player::PostUpdate()
{
	BROFILER_CATEGORY("Player PostUpdate", Profiler::Color::Black);

	return true;
}

void Player::CleanUp()
{
	LOG("---Player Deleted");

}