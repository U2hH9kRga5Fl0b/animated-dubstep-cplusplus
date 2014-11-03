/*
 * feasible.cpp
 *
 *  Created on: Oct 26, 2014
 *      Author: thallock
 */

#include "model/solution.h"

bool is_feasible(const Solution* solution)
{
	// solutions are assumed to be valid.

	if(solution->stops.rows() != solution->c->num_trucks)
	{
		err() << "bad num drivers" << std::endl;
		trap();
	}

	if (!solution->invs.in_capacity())
	{
		err() << "bad capacity" << std::endl;
		trap();
	}

	for (int d = 0; d < solution->stops.rows(); d++)
	{
		int len = solution->get_number_of_stops(d);
		int action = solution->get_action_index(d, len-1);
		if (get_truck_out_state(solution->c, action) != TRUCK_STATE_NONE)
		{
			err() << "bad ending state" << std::endl;
			trap();
		}

#if ENFORCE_TIME_WINDOWS
		for (int i = 0; i < len; i++)
		{
			int stop = solution->get_action_index(d, i);
			int time = solution->times.at(d, i);

			if (solution->get_city()->get_action(stop).begin_time > time
					|| solution->get_city()->get_action(stop).end_time < time)
			{
				err() << "invalid time" << std::endl;
				trap();
			}
		}
#endif
	}

	return true;
}
