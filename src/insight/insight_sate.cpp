/*
 * insight_sate.cpp
 *
 *  Created on: Nov 5, 2014
 *      Author: thallock
 */

#include "insight/insight_state.h"

insight_state::insight_state(const pairing_info* info_) :
	info { info_ },
	deliver_depots{new int[info->deliver_lens]},
	pickup_depots{new int[info->pickup_lens]},
	delivers_to_pickups{new int[info->deliver_lens]},
	unmatched_delivers{new int[info->deliver_lens]},
	unmatched_pickups{new int[info->pickup_lens]},
	embark{new interhub[info->city->num_trucks]},
	finish{new interhub[info->city->num_trucks]}
{
	// No matchings
	for (int i = 0; i < info->deliver_lens; i++)
	{
		deliver_depots[i] = info->get_nth_closest_deliver_depot(i, 0);
		unmatched_delivers[i] = deliver_depots[i];
		delivers_to_pickups[i] = -1;
	}
	for (int i = 0; i < info->pickup_lens; i++)
	{
		pickup_depots[i] = info->get_nth_closest_pickup_depot(i, 0);
		unmatched_pickups[i] = pickup_depots[i];
	}

	for (int i = 0; i < info->city->num_trucks; i++)
	{
		embark[i].end = finish[i].end = 0;
	}
}

insight_state::~insight_state()
{
	delete[] deliver_depots;
	delete[] pickup_depots;
	delete[] delivers_to_pickups;
	delete[] unmatched_delivers;
	delete[] unmatched_pickups;
	delete[] embark;
	delete[] finish;
}

std::ostream& operator<<(std::ostream& out, const insight_state& state)
{
	bool *aser = new bool[state.info->pickup_lens];
	for (int i = 0; i < state.info->pickup_lens; i++)
		aser[i] = false;

	out << "delivers:\n";
	for (int i = 0; i < state.info->deliver_lens; i++)
	{
		out << " " << std::setw(5) << state.deliver_depots[i];
		out << " " << std::setw(5) << state.info->deliver_actions[i];
		int pickup = state.delivers_to_pickups[i];
		if (pickup >= 0)
		{
			out << " " << std::setw(5) << state.info->pickup_actions[pickup];
			out << " " << std::setw(5) << state.pickup_depots[pickup];
			aser[pickup] = true;
		}
		else
		{
			out << " " << std::setw(5) << " ";
			out << " " << std::setw(5) << state.unmatched_delivers[i];
		}
		out << '\n';
	}

	out << "pickups:\n";
	for (int i = 0; i < state.info->pickup_lens; i++)
	{
		int pickup_depot = state.unmatched_pickups[i];
		if (pickup_depot < 0)
		{
			continue;
		}

		if (aser[i])
		{
			trap();
		}

		out << " " << std::setw(5) << state.pickup_depots[i];
		out << " " << std::setw(5) << state.info->pickup_actions[i];
		out << " " << std::setw(5) << pickup_depot;
		out << '\n';
	}
	out << std::endl;

	delete[] aser;
	return out;
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

		return current;
	}

	// we found an action
	current.path[0] = ac;
	ac = solution->get_action_index(driver, ++index);
	if (ac < 0)
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
		info { info_ },
		deliver_depots{new int[info->deliver_lens]},
		pickup_depots{new int[info->pickup_lens]},
		delivers_to_pickups{new int[info->deliver_lens]},
		unmatched_delivers{new int[info->deliver_lens]},
		unmatched_pickups{new int[info->pickup_lens]},
		embark{new interhub[info->city->num_trucks]},
		finish{new interhub[info->city->num_trucks]}
{
	const City* city = solution->get_city();
	for (int i=0;i<info->pickup_lens;i++)
	{
		deliver_depots[i] = -1;
		unmatched_pickups[i] = -1;
	}

	for (int i=0;i<info->deliver_lens;i++)
	{
		pickup_depots[i] = -1;
		delivers_to_pickups[i] = -1;
		unmatched_delivers[i] = -1;
	}

	for (int d = 0; d < solution->get_num_drivers(); d++)
	{
		int index = -1;
		do
		{
			interhub parsed = parse_inter_hub(solution, d, index);

//			log () << parsed << std::endl;

			if (parsed.is_beginning() && parsed.is_end())
			{
				embark[d] = parsed;
				finish[d] = parsed;
				break;
			}
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
				log() << "depot to depot" << std::endl;
				// These are ignored for now...
				continue;
			}

			int action1 = parsed.path[0];
			int action2 = parsed.path[1];

			if (state_is_full(city->get_action(action1, d).exit_state))
			{
				// this is a pickup
				pickup_depots[info->find_pickup_index(action1)] = parsed.end;
				unmatched_pickups[info->find_pickup_index(action1)] = parsed.begin;

				if (action2 >= 0)
				{
					trap();
				}
				continue;
			}

			// this is a deliver
			int deliver_index = info->find_deliver_index(action1);
			deliver_depots[deliver_index] = parsed.begin;

			if (parsed.has_both_requests())
			{
				// this is a pickup
				int pickup_index = info->find_pickup_index(action2);
				pickup_depots[pickup_index] = parsed.end;

				delivers_to_pickups[deliver_index] = pickup_index;
			}
			else
			{
				unmatched_delivers[deliver_index] = parsed.end;
			}
		}
		while (true);
	}
}




void insight_state::match(int deliver_index, int pickup_index)
{
	int old_pickup_index = delivers_to_pickups[deliver_index];
	if (old_pickup_index == pickup_index)
	{
		return;
	}

	if (pickup_index < 0)
	{
		unmatched_pickups[pickup_index] = info->get_nth_closest_pickup_depot(old_pickup_index, 0);
		delivers_to_pickups[deliver_index] = -1;
		unmatched_delivers[deliver_index] = info->get_nth_closest_deliver_depot(deliver_index, 0);
	}
	else
	{
		unmatched_pickups[pickup_index] = -1;
		delivers_to_pickups[deliver_index] = pickup_index;
		unmatched_delivers[deliver_index] = -1;
	}
}
