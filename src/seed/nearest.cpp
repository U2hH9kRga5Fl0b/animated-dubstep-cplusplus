/*
 * nearest.cpp
 *
 *  Created on: Oct 25, 2014
 *      Author: thallock
 */

#include "seed/seed.h"

#include "opts/depth_first_search.h"

bool search_for_path(Solution* solution, int driver, int start, int stop, int maxdepth, insertion& ins);

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
			insertion ins{d, sol->get_time_for_driver(d)};

			if (!canservice[d])
			{
				continue;
			}

			if (search_for_path(sol, d, -1, -1, 5, ins))
			{
				canserviceany = true;
				ins.apply(sol);
			}
		}
	} while (canserviceany);

	delete[] canservice;

	return sol;
}
