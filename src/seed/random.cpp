/*
 * random.cpp
 *
 *  Created on: Oct 24, 2014
 *      Author: thallock
 */

#include "seed/seed.h"

Solution* get_random_solution(const City* c)
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
		sol->append(d, 0, action);
	}

	int* tmp = new int[sol->get_city()->num_actions];
	bool* canservice = new bool[sol->get_city()->num_actions];
	for (int i = 0; i < sol->get_city()->num_actions; i++)
	{
		canservice[i] = true;
	}

	bool canserviceany = true;
	while (canserviceany)
	{
		canserviceany = false;
		for (int d = 0; d < c->num_trucks; d++)
		{
			if (!canservice[d])
			{
				continue;
			}

			int posslen = 0;
			int len = sol->get_number_of_stops(d);

			for (int p = 0; p < c->num_actions; p++)
			{
				int alt = c->possibles.at(sol->get_action_index(d, len-1), p);
				if (alt < 0)
				{
					break;
				}

				if (sol->already_serviced(alt))
				{
					continue;
				}

				tmp[posslen++] = alt;
				break;
			}

			if (posslen <= 0)
			{
				canservice[d] = false;
				continue;
			}

			canserviceany = true;
			sol->append(d, len, rand() % posslen);
		}
	}

	delete[] tmp;
	delete[] canservice;

	return sol;
}
