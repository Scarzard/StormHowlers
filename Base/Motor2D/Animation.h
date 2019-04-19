#ifndef __ANIMATION_H__
#define __ANIMATION_H__
#include <assert.h>
#include "SDL/include/SDL_rect.h"
#include "Scene.h"
#define MAX_FRAMES 50

class Animation
{
public:
	bool loop = true;
	float speed = 1.0f;
	SDL_Rect frames[MAX_FRAMES];


private:

	float current_frame;
	int last_frame = 0;
	int loops = 0;

public:

	Animation * LoadAnimation(const char* animationPath, const char* animationName)
	{

		Animation* animation = new Animation();

		bool anim = false;

		pugi::xml_document animationDocument;
		pugi::xml_parse_result result = animationDocument.load_file(animationPath);


		if (result == NULL)
		{
			LOG("Issue loading animation");
		}

		pugi::xml_node objgroup;
		for (objgroup = animationDocument.child("map").child("objectgroup"); objgroup; objgroup = objgroup.next_sibling("objectgroup"))
		{
			string name = objgroup.attribute("name").as_string();
			if (name == animationName)
			{
				anim = true;
				int x, y, h, w;

				for (pugi::xml_node sprite = objgroup.child("object"); sprite; sprite = sprite.next_sibling("object"))
				{
					x = sprite.attribute("x").as_int();
					y = sprite.attribute("y").as_int();
					w = sprite.attribute("width").as_int();
					h = sprite.attribute("height").as_int();

					animation->PushBack({ x, y, w, h });
				}

			}
		}
		if (anim = true)
			return animation;
		else
			return nullptr;

	}


	void PushBack(const SDL_Rect& rect)
	{
		if (last_frame < MAX_FRAMES)
			frames[last_frame++] = rect;
		assert(last_frame < MAX_FRAMES);
	}

	SDL_Rect& GetCurrentFrame(float dt)
	{
		float tmp = speed;
		
		if (App->scene->pause == false)
		{
			current_frame += tmp*dt;
		}

		if (current_frame < 0)
		{
			current_frame = 0;
		}

		if (current_frame >= last_frame)
		{
			current_frame = (loop) ? 0.0f : last_frame - 1;
			loops++;
		}

		return frames[(int)current_frame];
	}

	void SetCurrentFrame(int frame) {
		current_frame = frame;
	}

	bool Finished() const
	{
		return loops > 0;
	}

	void Reset()
	{
		current_frame = 0;
	}

	void ResetLoops()
	{
		loops = 0;
	}

	int GetLastFrame()
	{
		return last_frame;
	}
};

#endif