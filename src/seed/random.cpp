/*
 * random.cpp
 *
 *  Created on: Oct 24, 2014
 *      Author: thallock
 */

#include "seed/random.h"

Solution* get_random_solution(const City* c)
{
	int len = c->num_actions;
	Solution* sol = new Solution { c, c->num_actions };

	for (int d = 0; d < c->num_trucks; d++)
	{
		int action;
		do
		{
			action = rand() % c->num_actions;
			// no inventories or time windows
		} while (sol->already_serviced(action) || !c->is_start_action(action));
		sol->service(d, 0, action);
	}

	bool canserviceall = true;

	for (int s = 1; s < len && canserviceall; s++)
	{
		for (int d = 0; d < c->num_trucks && canserviceall; d++)
		{
			int next = -1;

			for (int p = 0; p < c->num_actions; p++)
			{
				// ok, not in order...
				int alt = c->possibles.at(sol->get_action(d, s-1), p);
				if (alt < 0)
				{
					break;
				}

				// no inventories or time windows
				if (sol->already_serviced(alt))
				{
					continue;
				}

				next = alt;
				break;
			}

			if (next < 0)
			{
				canserviceall = false;
				break;
			}

			sol->service(d, s, next);
		}
	}

	return sol;
}
