#ifndef __Timer_H__
#define __Timer_H__

#include "Defs.h"

class Timer
{
public:

	// Constructor
	Timer();

	void Start();
	void Stop();

	uint32 Read() const;
	float ReadSec() const;

	bool runningRead() const;

private:

	bool	running = false;
	uint32	started_at = 0;
	uint32	stopped_at = 0;
};

#endif //__Timer_H__