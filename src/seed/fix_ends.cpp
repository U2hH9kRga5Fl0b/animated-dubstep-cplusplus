/*
 * fix_ends.cpp
 *
 *  Created on: Nov 12, 2014
 *      Author: thallock
 */

#include "seed/seed.h"
#include "opts/find_path.h"

void fix_endings(Solution* ret_val)
{
	const City* city = ret_val->get_city();

	ret_val->ensure_valid();

	int numtrucks = ret_val->get_city()->num_trucks;
	for (int d = 0; d < numtrucks; d++)
	{
		int len = ret_val->get_number_of_stops(d);
		if (len == 0)
		{
			continue;
		}
		int penultimate = ret_val->get_action_index(d, len - 1);
		if (city->get_action(penultimate, d).exit_state == TRUCK_STATE_NONE)
		{
			// already done...
			continue;
		}

		int plen;
		int ptime;
		int path[2];
		find_path_between_requests(ret_val, d, penultimate, END_INDEX, &path[0], plen, ptime);

		for (int s = 0; s < 2; s++)
		{
			if (path[s] < 0)
			{
				break;
			}

			ret_val->ensure_valid();
			ret_val->append(d, len + s, path[s]);
			ret_val->ensure_valid();
		}
	}
}

