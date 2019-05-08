// ----------------------------------------------------
// Module.h
// Interface for all engine Module
// ----------------------------------------------------

#ifndef __Module_H__
#define __Module_H__

#include "PugiXml\src\pugixml.hpp"


using namespace std;
#include <iostream>
#include <utility>
#include <list> 
#include <iterator>
#include <string>
#include <vector>
#include <queue>

class MainApp;

class Module
{
public:

	Module() : active(false)
	{}

	void Init(bool check_active)
	{
		active = check_active;
	}

	// Called before render is available
	virtual bool Awake(pugi::xml_node&)
	{
		return true;
	}

	// Called before the first frame
	virtual bool Start()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PreUpdate()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool Update(float dt)
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PostUpdate()
	{
		return true;
	}

	// Called before quitting
	virtual bool CleanUp()
	{
		return true;
	}

	virtual bool Load(pugi::xml_node&)
	{
		return true;
	}

	virtual bool Save(pugi::xml_node&) const
	{
		return true;
	}

	void Enable()
	{
		if (!active)
		{
			active = true;
			Start();
		}
	}

	void Disable()
	{
		if (active)
		{
			active = false;
			CleanUp();
		}
	}

public:

	string		name;
	bool		active = false;

};

#endif // __Module_H__
