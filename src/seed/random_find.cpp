/*
 * random_find.cpp
 *
 *  Created on: Nov 3, 2014
 *      Author: thallock
 */

#include "seed/seed.h"

#include "opts/find_path.h"

Solution* get_random_solution_find(const City* c)
{
	Solution* sol = new Solution { c, c->num_actions };

	int* tmp = new int[c->num_requests];
	for (int i = 0; i < c->num_requests; i++)
	{
		tmp[i] = c->first_request_index + i;
	}

	std::random_shuffle(&tmp[0], &tmp[c->num_requests]);

	int d = 0;
	for (int i = 0; i < c->num_requests; i++)
	{
		int length = sol->get_number_of_stops(d);
		int pa = sol->get_action_index(d, length-1);
		int na = tmp[i];

		int time;
		int len;
		int path[MAX_PATH];
		find_path_between_requests(sol, d, pa, na, &path[0], len, time);

		for (int j = 0; j < len; j++)
		{
			sol->append(d, length + j, path[j]);
		}
		sol->append(d, length + len, tmp[i]);

		d = (d + 1) % c->num_trucks;
	}

	delete[] tmp;

	return sol;
}
