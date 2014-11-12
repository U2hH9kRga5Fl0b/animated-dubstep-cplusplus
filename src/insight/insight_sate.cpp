/*
 * insight_sate.cpp
 *
 *  Created on: Nov 5, 2014
 *      Author: thallock
 */

#include "insight/insight_state.h"

insight_state::insight_state(const pairing_info* info_) :
		info { info_ }
{
		deliver_depots = new int[info->deliver_lens];
		delivers_to_pickups = new int[info->deliver_lens];
		pickup_depots = new int[info->pickup_lens];

		for (int i = 0; i < info->deliver_lens[s]; i++)
		{
			deliver_depots[i] = info->get_nth_closest_deliver_depot(i, 0);
			delivers_to_pickups[s][i] = -1;
		}
		for (int i = 0; i < info->pickup_lens[s]; i++)
			pickup_depots[i] = info->get_nth_closest_pickup_depot(i, 0);
}

insight_state::~insight_state()
{
	delete[] deliver_depots;
	delete[] pickup_depots;
	delete[] delivers_to_pickups;
}

friend std::ostream& operator<<(std::ostream& out, const insight_state& state)
{

}

namespace
{
int get_depot(const City* city, int index)
{
	if (index > city->first_request_index)
	{
		return -1;
	}
	if (index < 0)
	{
		return -1;
	}
	if (index < city->first_stagingarea_index)
	{
		if (index >= 0 && index / NUM_ACTIONS_PER_FILL != city->get_action(index).location)
		{
			trap();
		}
		return index / NUM_ACTIONS_PER_FILL;
	}
	else
	{
		if (index >= 0 && (index - city->first_stagingarea_index) / NUM_ACTIONS_PER_YARD != city->get_action(index).location - city->num_landfills)
		{
			trap();
		}
		return (index - city->first_stagingarea_index) / NUM_ACTIONS_PER_YARD;
	}
}


interhub parse_inter_hub(const Solution* solution, int driver, int& index)
{
	interhub current;
	current.begin = get_depot(solution->get_city(), solution->get_action_index(driver, index));

	int ac;
	int depot;

	int start = 0;

	// while we are talking about actions at the same depot
	while (true)
	{
		ac = solution->get_action_index(driver, ++index);
		if (ac < 0)
		{
			if (DEBUG && index != solution->get_number_of_stops(driver))
			{
				trap();
			}
			// hit end of the route with no requests
			return current;
		}

		depot = get_depot(solution->get_city(), ac);
		if (depot < 0 || depot != current.begin)
		{
			// hit request or other depot
			break;
		}

		if (DEBUG && (++start > 4 || (index >= solution->get_number_of_stops(driver))))
		{
			trap();
		}
	}

	if (depot >= 0)
	{
		// there was a depot with no action
		current.end = depot;

		log() << "depot to depot" << std::endl;

		return current;
	}

	// we found an action
	current.path[0] = ac;
	ac = solution->get_action_index(driver, ++index);
	if (DEBUG && ac < 0)
	{
		// one action followed by the end of the route....
		if (DEBUG && index != solution->get_number_of_stops(driver))
		{
			trap();
		}

		return current;
	}
	depot = get_depot(solution->get_city(), ac);
	if (depot >= 0)
	{
		// one action followed by depot
		current.end = depot;
		return current;
	}

	// both actions used.
	current.path[1] = ac;
	ac = solution->get_action_index(driver, ++index);
	if (DEBUG && ac < 0)
	{
		// can't have two actions before done...
		trap();
	}

	current.end = get_depot(solution->get_city(), ac);
	if (DEBUG && current.end < 0)
	{
		// can't have 3 non-depot actions in a row...
		trap();
	}
	return current;







#if 0




	bool hit_depot = false;
	for (int i=1;i<5;i++)
	{
		int ac = solution->get_action_index(driver, index + i);
		if (ac < 0)
		{
			// end of route
			if (current.end < 0)
			{
				return current;
			}
			else
			{
				current.end = ac;
				return current;
			}
		}
		int depot = get_depot(solution->get_city(), ac);
		if (depot < 0)
		{
			// not a depot
			if (i == 1)
			{
				current.path[0] = ac; // this is the actual action index...
				continue;
			}
			else if (i == 2)
			{
				current.path[0] = ac; // this is the actual action index...
				continue;
			}
			else
			{
				trap();
			}
		}
		else
		{
			// this is a depot
			if (index == 1 && depot == current.begin)
			{
				trap();
			}

			if (!hit_depot)
			{
				current.end = depot;
				hit_depot = true;
				continue;
			}
			else
			{
				if (current.end == depot)
				{
					continue;
				}
				else
				{
					index = index + i - 1;
					return current;
				}
			}
		}
	}


	trap();
	int na = solution->get_action_index(driver, index);
	if (na < 0)
	{
		// end of route
		current.end = na;
		return current;
	}

	int next = get_depot(solution->get_city(), na);
	if (next == current.begin)
	{
		trap();
	}
	if (next < 0)
	{
		current.path[0] = na;
	}
	na = solution->get_action_index(driver, ++index);
	if (na < 0)
	{
		// end of route
		current.end = na;
		return current;
	}
#endif
}


}




insight_state::insight_state(const pairing_info* info_, const Solution* solution) :
		info { info_ }
{
	const City* city = solution->get_city();

	for (int d = 0; d < solution->get_num_drivers(); d++)
	{
		int len = solution->get_number_of_stops(d);

		int index = -1;
		do
		{
			interhub parsed = parse_inter_hub(solution, d, index);
			if (parsed.is_beginning())
			{
				embark[d] = parsed;
				continue;
			}
			if (parsed.is_end())
			{
				finish[d] = parsed;
				break;
			}
			if (!parsed.has_requests())
			{
				// These are ignored for now...
				continue;
			}

			int action1 = parsed.path[0];
			int action2 = parsed.path[1];

			if (state_is_full(city->get_action(action1, d).exit_state))
			{
				// this is a pickup
				int dumpster_index = d2i(city->get_action(action1, d).exit_state);
				pickup_depots[dumpster_index][info->find_pickup_index(dumpster_index, action1)] = parsed.end; // this is assigning the actual depot...

				continue;
			}

			// this is a deliver
			int deliver_dumpster = d2i(city->get_action(action1, d).entr_state);
			int deliver_index = info->find_deliver_index(deliver_dumpster, action1);
			deliver_depots[deliver_index] = parsed.begin; // this is assigning the actual depot...

			if (parsed.has_both_requests())
			{
				// this is a pickup
				int dumpster_index = d2i(city->get_action(action2, d).exit_state);
				int pickup_index = info->find_pickup_index(dumpster_index, action2);
				pickup_depots[dumpster_index][pickup_index] = parsed.end; // this is assigning the actual depot...



//				this is the end of the world.
//				how could this be a pickup index, when we don't even know the dumpster size...
				delivers_to_pickups[deliver_dumpster][deliver_index] = pickup_index;


				trap();
			}
			else

			int *deliver_depots[4];
			int *pickup_depots[4];

			int *delivers_to_pickups[4];

			int *unmatched_delivers[4];
			int *unmatched_pickups[4];

			interhub *embark;
			interhub *finish;
		}
		while (true);
	}
}
}
