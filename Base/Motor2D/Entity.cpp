#include "Defs.h"
#include "Log.h"
#include <iostream>
#include <list>
#include <iterator>
using namespace std;

#include "App.h"
#include "Textures.h"
#include "Map.h"
#include "Input.h"
#include "Transitions.h"
#include "Scene.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "EntityManager.h"
#include "Entity.h"


Entity::Entity()
{
}

Entity::Entity(entityType type) : type(type)
{
}

Entity::~Entity()
{
}

void Entity::Draw(float dt)
{
	if (flip)
	{
		App->render->Blit(App->entitymanager->texture, position.first, position.second, &(Current_Animation->GetCurrentFrame(dt)), SDL_FLIP_HORIZONTAL);
	}
	else
	{
		App->render->Blit(App->entitymanager->texture, position.first, position.second, &(Current_Animation->GetCurrentFrame(dt)), SDL_FLIP_NONE);
	}
}

void Entity::Collider_Overlay()
{
	SDL_Rect ObjectRect;
	SDL_Rect result;

	list<ObjectsGroup*>::iterator object;
	list<ObjectsData*>::iterator objectdata;

	for ( object = App->map->data.objLayers.begin(); object != App->map->data.objLayers.end(); object++) //objects colliders
	{
		if ((*object)->name == "Collision")
		{
			for (objectdata = (*object)->objects.begin(); objectdata != (*object)->objects.end(); objectdata++)
			{
				ObjectRect.x = (*objectdata)->x;
				ObjectRect.y = (*objectdata)->y;
				ObjectRect.w = (*objectdata)->width;
				ObjectRect.h = (*objectdata)->height;

				if (SDL_IntersectRect(&Collider, &ObjectRect, &result))
				{
					if ((*objectdata)->name == "Wall")
					{
					}
				}
			}
		}
	}
}

void Entity::PositionCollider()
{
	Collider.x = position.first;
	Collider.y = position.second;
	Collider.w = size.first;
	Collider.h = size.second;
}