/*
 * nearest_valuable.cpp
 *
 *  Created on: Oct 25, 2014
 *      Author: thallock
 */

#include "seed/random.h"

bool apply_nearest_valuable(Solution*sol, int driver, int stop)
{
	int lastaction = sol->get_action(driver, stop - 1);

	// This first block won't need to be executed when the dfs is smart about which action to descent down...
	{
		int best = -1;
		int best_duration = INT_MAX;

		for (int p = 0; p < sol->c->possibles.cols(); p++)
		{
			// ok, not in order...
			int alt = sol->c->possibles.at(lastaction, p);
			if (alt < 0)
			{
				break;
			}
			if (!sol->c->get_action(alt).value)
			{
				continue;
			}

			// no inventories or time windows
			if (sol->already_serviced(alt))
			{
				continue;
			}

			if (sol->c->get_time_to(lastaction, alt) < best_duration)
			{
				best = alt;
				best_duration = sol->c->get_time_to(lastaction, alt);
			}
		}

		if (best >= 0)
		{
			sol->service(driver, stop, best);
			return true;
		}
	}

#define PATH_LENGTH_SEARCH 4
	int path[PATH_LENGTH_SEARCH];
	// depth first search to find the nearest stop of value...
	int time = get_best_path(sol, lastaction, &path[0], PATH_LENGTH_SEARCH);
	if (time < 0)
	{
		std::cout << (*sol) << std::endl;
		std::cout << "last action= " << sol->c->get_action(lastaction) << std::endl;
		int time = get_best_path(sol, lastaction, &path[0], PATH_LENGTH_SEARCH);




		return false;
	}

	for (int i = 0; i < PATH_LENGTH_SEARCH; i++)
	{
		sol->service(driver, stop + i, path[i]);
		if (sol->c->get_action(path[i]).value)
		{
			break;
		}
	}
	return true;
}
