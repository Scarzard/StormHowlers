#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Input.h"
#include "Window.h"
#include "Player.h"
#include "Brofiler\Brofiler.h"
#include "SDL/include/SDL.h"

#define MAX_KEYS 300
#define DEATH_ZONE 6400
#define DEATHZONE_TWO 12800

Input::Input() : Module()
{
	name.assign("input");

	keyboard = new j1KeyState[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(j1KeyState) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(j1KeyState) * NUM_MOUSE_BUTTONS);
}

// Destructor
Input::~Input()
{
	delete[] keyboard;
}

// Called before render is available
bool Input::Awake(pugi::xml_node& config)
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);
	SDL_Init(SDL_INIT_GAMECONTROLLER);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	if (SDL_NumJoysticks() < 1)
	{
		LOG("Warning: No joysticks connected!\n");
	}
	else
	{
		if (SDL_NumJoysticks() == 1) //Here we have to put instead of a 2 a 1 (im doing it with ps4 controllers and its bugged, with XBOX controllers will work with a 1) 
		{
			//If there is only 1 controller connected
			App->player1->gamepad.GameController = SDL_GameControllerOpen(0);
			App->player1->gamepad.joy = SDL_JoystickOpen(0);
		}
		else if (SDL_NumJoysticks() == 2) //Here we have to put instead of a 4 a 2 (im doing it with ps4 controllers and its bugged, with XBOX controllers will work with a 2)
		{
			// If there are 2 controllers connected
			App->player1->gamepad.GameController = SDL_GameControllerOpen(0);
			App->player1->gamepad.joy = SDL_JoystickOpen(0);

			App->player2->gamepad.GameController = SDL_GameControllerOpen(1);
			App->player2->gamepad.joy = SDL_JoystickOpen(1);
		}


	}

	return ret;
}

// Called before the first frame
bool Input::Start()
{
	SDL_StopTextInput();
	return true;
}

// Called each loop iteration
bool Input::PreUpdate()
{
	BROFILER_CATEGORY("Input PreUpdate", Profiler::Color::GreenYellow);

	static SDL_Event event;
	
	const Uint8* keys = SDL_GetKeyboardState(NULL);

	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	for(int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		if(mouse_buttons[i] == KEY_DOWN)
			mouse_buttons[i] = KEY_REPEAT;

		if(mouse_buttons[i] == KEY_UP)
			mouse_buttons[i] = KEY_IDLE;
	}

	for (int i = 0; i < SDL_NumJoysticks(); ++i) //Remove the /2 for XBOX controller (with ps4 works weirdly)
	{
		if (SDL_IsGameController(i))
		{
			if (i == 0)
			{
				if (SDL_IsGameController(i))
				{
					App->player1->gamepad.GameController = SDL_GameControllerOpen(i);
					App->player1->gamepad.joy = SDL_JoystickOpen(i);
					if (SDL_GameControllerGetAttached(App->player1->gamepad.GameController))
					{
						// CONTROLLER 1 ----------------------------------------------------------------------------------------
						App->player1->gamepad.State[UP] = SDL_GameControllerGetButton(App->player1->gamepad.GameController, SDL_CONTROLLER_BUTTON_DPAD_UP);
						App->player1->gamepad.State[DOWN] = SDL_GameControllerGetButton(App->player1->gamepad.GameController, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
						App->player1->gamepad.State[LEFT] = SDL_GameControllerGetButton(App->player1->gamepad.GameController, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
						App->player1->gamepad.State[RIGHT] = SDL_GameControllerGetButton(App->player1->gamepad.GameController, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
						App->player1->gamepad.State[BUTTON_A] = SDL_GameControllerGetButton(App->player1->gamepad.GameController, SDL_CONTROLLER_BUTTON_A);
						App->player1->gamepad.State[BUTTON_B] = SDL_GameControllerGetButton(App->player1->gamepad.GameController, SDL_CONTROLLER_BUTTON_B);
						App->player1->gamepad.State[BUTTON_X] = SDL_GameControllerGetButton(App->player1->gamepad.GameController, SDL_CONTROLLER_BUTTON_X);
						App->player1->gamepad.State[BUTTON_Y] = SDL_GameControllerGetButton(App->player1->gamepad.GameController, SDL_CONTROLLER_BUTTON_Y);
						App->player1->gamepad.State[START] = SDL_GameControllerGetButton(App->player1->gamepad.GameController, SDL_CONTROLLER_BUTTON_START);
						App->player1->gamepad.State[BACK] = SDL_GameControllerGetButton(App->player1->gamepad.GameController, SDL_CONTROLLER_BUTTON_BACK);
						App->player1->gamepad.State[LT] = SDL_GameControllerGetButton(App->player1->gamepad.GameController, SDL_CONTROLLER_BUTTON_LEFTSTICK);
						App->player1->gamepad.State[RT] = SDL_GameControllerGetButton(App->player1->gamepad.GameController, SDL_CONTROLLER_BUTTON_RIGHTSTICK);
						App->player1->gamepad.State[LB] = SDL_GameControllerGetButton(App->player1->gamepad.GameController, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
						App->player1->gamepad.State[RB] = SDL_GameControllerGetButton(App->player1->gamepad.GameController, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
					
						App->player1->gamepad.LeftAxisX = SDL_JoystickGetAxis(App->player1->gamepad.joy, SDL_CONTROLLER_AXIS_LEFTX);
						App->player1->gamepad.LeftAxisY = SDL_JoystickGetAxis(App->player1->gamepad.joy, SDL_CONTROLLER_AXIS_LEFTY);
						App->player1->gamepad.Connected = true;

					}
					else
					{
						SDL_GameControllerClose(App->player1->gamepad.GameController);
						App->player1->gamepad.GameController = nullptr;
						App->player1->gamepad.Connected = false;
					}
				}
			}
			else if (i < 1)
			{
				App->player2->gamepad.Connected = false;
				SDL_GameControllerClose(App->player2->gamepad.GameController);
				App->player2->gamepad.GameController = nullptr;
			}
			else if (i == 1 || i == 0 && App->player1->gamepad.Connected == false)
			{
				if (SDL_IsGameController(i))
				{
					App->player2->gamepad.GameController = SDL_GameControllerOpen(1);
					App->player2->gamepad.joy = SDL_JoystickOpen(1);
					if (SDL_GameControllerGetAttached(App->player2->gamepad.GameController))
					{
						// CONTROLLER 2 ------------------------------------------------------------------------------------------

						App->player2->gamepad.State[UP] = SDL_GameControllerGetButton(App->player2->gamepad.GameController, SDL_CONTROLLER_BUTTON_DPAD_UP);
						App->player2->gamepad.State[DOWN] = SDL_GameControllerGetButton(App->player2->gamepad.GameController, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
						App->player2->gamepad.State[LEFT] = SDL_GameControllerGetButton(App->player2->gamepad.GameController, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
						App->player2->gamepad.State[RIGHT] = SDL_GameControllerGetButton(App->player2->gamepad.GameController, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
						App->player2->gamepad.State[BUTTON_A] = SDL_GameControllerGetButton(App->player2->gamepad.GameController, SDL_CONTROLLER_BUTTON_A);
						App->player2->gamepad.State[BUTTON_B] = SDL_GameControllerGetButton(App->player2->gamepad.GameController, SDL_CONTROLLER_BUTTON_B);
						App->player2->gamepad.State[BUTTON_X] = SDL_GameControllerGetButton(App->player2->gamepad.GameController, SDL_CONTROLLER_BUTTON_X);
						App->player2->gamepad.State[BUTTON_Y] = SDL_GameControllerGetButton(App->player2->gamepad.GameController, SDL_CONTROLLER_BUTTON_Y);
						App->player2->gamepad.State[START] = SDL_GameControllerGetButton(App->player2->gamepad.GameController, SDL_CONTROLLER_BUTTON_START);
						App->player2->gamepad.State[BACK] = SDL_GameControllerGetButton(App->player2->gamepad.GameController, SDL_CONTROLLER_BUTTON_BACK);
						App->player2->gamepad.State[LT] = SDL_GameControllerGetButton(App->player2->gamepad.GameController, SDL_CONTROLLER_BUTTON_LEFTSTICK);
						App->player2->gamepad.State[RT] = SDL_GameControllerGetButton(App->player2->gamepad.GameController, SDL_CONTROLLER_BUTTON_RIGHTSTICK);
						App->player2->gamepad.State[LB] = SDL_GameControllerGetButton(App->player2->gamepad.GameController, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
						App->player2->gamepad.State[RB] = SDL_GameControllerGetButton(App->player2->gamepad.GameController, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);

						App->player2->gamepad.LeftAxisX = SDL_JoystickGetAxis(App->player2->gamepad.joy, SDL_CONTROLLER_AXIS_LEFTX);
						App->player2->gamepad.LeftAxisY = SDL_JoystickGetAxis(App->player2->gamepad.joy, SDL_CONTROLLER_AXIS_LEFTY);

						App->player2->gamepad.Connected = true;
					}
					else
					{
						SDL_GameControllerClose(App->player2->gamepad.GameController);
						App->player2->gamepad.GameController = nullptr;
						App->player2->gamepad.Connected = false;
					}
				}
			}
		}
	}

	for (int i = 0; i < MAX_BUTTONS; ++i)
	{
		if (App->player1->gamepad.State[i] == 1) {
			if (App->player1->gamepad.Controller[i] == KEY_IDLE)
				App->player1->gamepad.Controller[i] = KEY_DOWN;
			else
				App->player1->gamepad.Controller[i] = KEY_REPEAT;
		}
		else
		{
			if (App->player1->gamepad.Controller[i] == KEY_REPEAT || App->player1->gamepad.Controller[i] == KEY_DOWN)
				App->player1->gamepad.Controller[i] = KEY_UP;
			else
				App->player1->gamepad.Controller[i] = KEY_IDLE;
		}

		if (App->player2->gamepad.State[i] == 1) {
			if (App->player2->gamepad.Controller[i] == KEY_IDLE)
				App->player2->gamepad.Controller[i] = KEY_DOWN;
			else
				App->player2->gamepad.Controller[i] = KEY_REPEAT;
		}
		else
		{
			if (App->player2->gamepad.Controller[i] == KEY_REPEAT || App->player2->gamepad.Controller[i] == KEY_DOWN)
				App->player2->gamepad.Controller[i] = KEY_UP;
			else
				App->player2->gamepad.Controller[i] = KEY_IDLE;
		}

	}

	//CHECK Left Axis X & Y
	if (App->player1->gamepad.LeftAxisX > DEATH_ZONE)
	{
		App->player1->gamepad.JoystickState[3] = true;
	}
	else
	{
		App->player1->gamepad.JoystickState[3] = false;
	}

	if (App->player1->gamepad.LeftAxisX < -DEATH_ZONE)
	{
		App->player1->gamepad.JoystickState[2] = true;
	}
	else
	{
		App->player1->gamepad.JoystickState[2] = false;
	}

	if (App->player1->gamepad.LeftAxisY < -DEATH_ZONE)
	{
		App->player1->gamepad.JoystickState[0] = true;
	}
	else
	{
		App->player1->gamepad.JoystickState[0] = false;
	}
	
	if (App->player1->gamepad.LeftAxisY > DEATH_ZONE)
	{
		App->player1->gamepad.JoystickState[1] = true;
	}
	else
	{
		App->player1->gamepad.JoystickState[1] = false;
	}

	//CHECK P2 Left Axis X & Y
	if (App->player2->gamepad.LeftAxisX > DEATH_ZONE)
	{
		App->player2->gamepad.JoystickState[3] = true;
	}
	else
	{
		App->player2->gamepad.JoystickState[3] = false;
	}

	if (App->player2->gamepad.LeftAxisX < -DEATH_ZONE)
	{
		App->player2->gamepad.JoystickState[2] = true;
	}
	else
	{
		App->player2->gamepad.JoystickState[2] = false;
	}

	if (App->player2->gamepad.LeftAxisY < -DEATH_ZONE)
	{
		App->player2->gamepad.JoystickState[0] = true;
	}
	else
	{
		App->player2->gamepad.JoystickState[0] = false;
	}

	if (App->player2->gamepad.LeftAxisY > DEATH_ZONE)
	{
		App->player2->gamepad.JoystickState[1] = true;
	}
	else
	{
		App->player2->gamepad.JoystickState[1] = false;
	}

	for (int i = 0; i < 4; i++)
	{
		if (App->player1->gamepad.JoystickState[i] == true) {
			if (App->player1->gamepad.Controller[i] == KEY_IDLE)
				App->player1->gamepad.Controller[i] = KEY_DOWN;
			else
				App->player1->gamepad.Controller[i] = KEY_REPEAT;
		}
		else
		{
			if (App->player1->gamepad.Controller[i] == KEY_REPEAT || App->player1->gamepad.Controller[i] == KEY_DOWN)
				App->player1->gamepad.Controller[i] = KEY_UP;
			else
				App->player1->gamepad.Controller[i] = KEY_IDLE;
		}

		if (App->player2->gamepad.JoystickState[i] == true) {
			if (App->player2->gamepad.Controller[i] == KEY_IDLE)
				App->player2->gamepad.Controller[i] = KEY_DOWN;
			else
				App->player2->gamepad.Controller[i] = KEY_REPEAT;
		}
		else
		{
			if (App->player2->gamepad.Controller[i] == KEY_REPEAT || App->player2->gamepad.Controller[i] == KEY_DOWN)
				App->player2->gamepad.Controller[i] = KEY_UP;
			else
				App->player2->gamepad.Controller[i] = KEY_IDLE;
		}
	}


	while(SDL_PollEvent(&event) != 0)
	{
		switch(event.type)
		{
			case SDL_QUIT:
				windowEvents[WE_QUIT] = true;
			break;

			case SDL_WINDOWEVENT:
				switch(event.window.event)
				{
					//case SDL_WINDOWEVENT_LEAVE:
					case SDL_WINDOWEVENT_HIDDEN:
					case SDL_WINDOWEVENT_MINIMIZED:
					case SDL_WINDOWEVENT_FOCUS_LOST:
					windowEvents[WE_HIDE] = true;
					break;

					//case SDL_WINDOWEVENT_ENTER:
					case SDL_WINDOWEVENT_SHOWN:
					case SDL_WINDOWEVENT_FOCUS_GAINED:
					case SDL_WINDOWEVENT_MAXIMIZED:
					case SDL_WINDOWEVENT_RESTORED:
					windowEvents[WE_SHOW] = true;
					break;
				}
			break;

			case SDL_MOUSEBUTTONDOWN:
				mouse_buttons[event.button.button - 1] = KEY_DOWN;
				//LOG("Mouse button %d down", event.button.button-1);
			break;

			case SDL_MOUSEBUTTONUP:
				mouse_buttons[event.button.button - 1] = KEY_UP;
				//LOG("Mouse button %d up", event.button.button-1);
			break;

			case SDL_MOUSEMOTION:
				int scale = App->win->GetScale();
				mouse_motion_x = event.motion.xrel / scale;
				mouse_motion_y = event.motion.yrel / scale;
				mouse_x = event.motion.x / scale;
				mouse_y = event.motion.y / scale;
				//LOG("Mouse motion x %d y %d", mouse_motion_x, mouse_motion_y);
			break;
		}
	}

	return true;
}

// Called before quitting
bool Input::CleanUp()
{
	LOG("Quitting SDL event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);

	SDL_GameControllerClose(App->player1->gamepad.GameController);
	App->player1->gamepad.GameController = NULL;

	SDL_GameControllerClose(App->player2->gamepad.GameController);
	App->player2->gamepad.GameController = NULL;

	return true;
}

// ---------
bool Input::GetWindowEvent(j1EventWindow ev)
{
	return windowEvents[ev];
}

void Input::GetMousePosition(int& x, int& y)
{
	x = mouse_x;
	y = mouse_y;
}

void Input::GetMouseMotion(int& x, int& y)
{
	x = mouse_motion_x;
	y = mouse_motion_y;
}