/*
 * nearest_valuable.cpp
 *
 *  Created on: Oct 25, 2014
 *      Author: thallock
 */

#include "seed/random.h"


#define PATH_LENGTH_SEARCH 5

bool apply_nearest_valuable(Solution*sol, int driver, int stop)
{
	int lastaction = sol->get_action(driver, stop - 1);

	int path[PATH_LENGTH_SEARCH];
	// depth first search to find the nearest stop of value...
	int time = get_best_path(sol, lastaction, &path[0], PATH_LENGTH_SEARCH);
	if (time < 0)
	{
//		std::cout << (*sol) << std::endl;
//		std::cout << "last action= " << sol->c->get_action(lastaction) << std::endl;
//		int time = get_best_path(sol, lastaction, &path[0], PATH_LENGTH_SEARCH);
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
