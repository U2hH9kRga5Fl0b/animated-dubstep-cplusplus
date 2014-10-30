/*
 * snapshot.cpp
 *
 *  Created on: Oct 25, 2014
 *      Author: thallock
 */

#include "model/solution.h"

void take_still_shot(const Solution* solution, int time, Coord *coords, int *actions)
{
	int num_drivers = solution->get_num_drivers();
	for (int i = 0; i < num_drivers; i++)
	{
		if (actions == nullptr)
		{
			coords[i] = solution->interpolate_location_at(i, time, nullptr);
		}
		else
		{
			coords[i] = solution->interpolate_location_at(i, time, &actions[i]);
		}
	}
}

