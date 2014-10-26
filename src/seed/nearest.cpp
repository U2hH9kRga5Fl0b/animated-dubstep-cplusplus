/*
 * nearest.cpp
 *
 *  Created on: Oct 25, 2014
 *      Author: thallock
 */

#include "seed/random.h"

Solution* get_nearest_seed(const City* c)
{
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

	bool *canservice = new bool[c->num_trucks];

	for (int d = 0; d < c->num_trucks; d++)
		canservice[d] = true;

	bool canserviceany;
	do
	{
		canserviceany = false;
		for (int d = 0; d < c->num_trucks; d++)
		{
			if (!canservice[d])
			{
				continue;
			}
			canserviceany |= (canservice[d] = apply_nearest_valuable(sol, d, sol->get_length(d)));
		}
	} while (canserviceany);

	delete[] canservice;

	return sol;
}
