#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Input.h"
#include "Window.h"
#include "Brofiler\Brofiler.h"
#include "SDL/include/SDL.h"

#define MAX_KEYS 300

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
		if (SDL_NumJoysticks() == 2) //Here we have to put instead of a 2 a 1 (im doing it with ps4 controllers and its bugged, with XBOX controllers will work with a 1) 
		{
			//If therre is only 1 controller connected
			GameController1 = SDL_GameControllerOpen(0);
			joy1 = SDL_JoystickOpen(0);
		}
		else if (SDL_NumJoysticks() == 4) //Here we have to put instead of a 4 a 2 (im doing it with ps4 controllers and its bugged, with XBOX controllers will work with a 2)
		{
			// If there are 2 controllers connected
			GameController1 = SDL_GameControllerOpen(0);
			joy1 = SDL_JoystickOpen(0);

			GameController2 = SDL_GameControllerOpen(1);
			joy2 = SDL_JoystickOpen(1);
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

	for (int i = 0; i < SDL_NumJoysticks() / 2; ++i) //Remove the /2 for XBOX controller (with ps4 works weirdly)
	{
		if (SDL_IsGameController(i))
		{
			if (i == 0)
			{
				if (SDL_IsGameController(i))
				{
					GameController1 = SDL_GameControllerOpen(i);
					joy1 = SDL_JoystickOpen(i);
					if (SDL_GameControllerGetAttached(GameController1))
					{
						// CONTROLLER 1 ----------------------------------------------------------------------------------------
						controller1_state[UP] = SDL_GameControllerGetButton(GameController1, SDL_CONTROLLER_BUTTON_DPAD_UP);
						controller1_state[DOWN] = SDL_GameControllerGetButton(GameController1, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
						controller1_state[LEFT] = SDL_GameControllerGetButton(GameController1, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
						controller1_state[RIGHT] = SDL_GameControllerGetButton(GameController1, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
						controller1_state[BUTTON_A] = SDL_GameControllerGetButton(GameController1, SDL_CONTROLLER_BUTTON_A);
						controller1_state[BUTTON_B] = SDL_GameControllerGetButton(GameController1, SDL_CONTROLLER_BUTTON_B);
						controller1_state[BUTTON_X] = SDL_GameControllerGetButton(GameController1, SDL_CONTROLLER_BUTTON_X);
						controller1_state[BUTTON_Y] = SDL_GameControllerGetButton(GameController1, SDL_CONTROLLER_BUTTON_Y);
						controller1_state[START] = SDL_GameControllerGetButton(GameController1, SDL_CONTROLLER_BUTTON_START);

						P1LAxisX = SDL_JoystickGetAxis(joy1, SDL_CONTROLLER_AXIS_LEFTX);
						P1LAxisY = SDL_JoystickGetAxis(joy1, SDL_CONTROLLER_AXIS_LEFTY);
						P1connected = true;

					}
					else
					{
						SDL_GameControllerClose(GameController1);
						GameController1 = nullptr;
						P1connected = false;
					}
				}
			}
			else if (i < 1)
			{
				P2connected = false;
				SDL_GameControllerClose(GameController2);
				GameController2 = nullptr;
			}
			else if (i == 1 || i == 0 && P1connected == false)
			{
				if (SDL_IsGameController(i))
				{
					GameController2 = SDL_GameControllerOpen(1);
					joy2 = SDL_JoystickOpen(1);
					if (SDL_GameControllerGetAttached(GameController2))
					{
						// CONTROLLER 2 ------------------------------------------------------------------------------------------

						controller2_state[UP] = SDL_GameControllerGetButton(GameController2, SDL_CONTROLLER_BUTTON_DPAD_UP);
						controller2_state[DOWN] = SDL_GameControllerGetButton(GameController2, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
						controller2_state[LEFT] = SDL_GameControllerGetButton(GameController2, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
						controller2_state[RIGHT] = SDL_GameControllerGetButton(GameController2, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
						controller2_state[BUTTON_A] = SDL_GameControllerGetButton(GameController2, SDL_CONTROLLER_BUTTON_A);
						controller2_state[BUTTON_B] = SDL_GameControllerGetButton(GameController2, SDL_CONTROLLER_BUTTON_B);
						controller2_state[BUTTON_X] = SDL_GameControllerGetButton(GameController2, SDL_CONTROLLER_BUTTON_X);
						controller2_state[BUTTON_Y] = SDL_GameControllerGetButton(GameController2, SDL_CONTROLLER_BUTTON_Y);
						controller2_state[START] = SDL_GameControllerGetButton(GameController2, SDL_CONTROLLER_BUTTON_START);

						P2LAxisX = SDL_JoystickGetAxis(joy2, SDL_CONTROLLER_AXIS_LEFTX);
						P2LAxisY = SDL_JoystickGetAxis(joy2, SDL_CONTROLLER_AXIS_LEFTY);

						P2connected = true;
					}
					else
					{
						SDL_GameControllerClose(GameController2);
						GameController2 = nullptr;
						P2connected = false;
					}
				}
			}
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