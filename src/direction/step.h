/*
 * step.h
 *
 *  Created on: Nov 2, 2014
 *      Author: thallock
 */

#ifndef STEP_H_
#define STEP_H_

#include "direction/location.h"

#include <list>

class step
{
public:
	step(std::istream& input);

	bool valid()
	{
		return duration >= 0 && distance >= 0;
	}

	friend std::ostream& operator<<(std::ostream& out, const step& s)
	{
		return out << "{[d:" << s.distance << "][t:" << s.duration << "]}";
	}

	int duration;
	int distance;

	location begin;
	location end;

	std::list<location> path;
};

#endif /* STEP_H_ */
