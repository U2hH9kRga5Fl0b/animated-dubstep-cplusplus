/*
 * find_path.cpp
 *
 *  Created on: Nov 3, 2014
 *      Author: thallock
 */

#include "opts/find_path.h"

namespace
{
	struct landfill_comparator
	{
		int a1;
		const City* city;
		landfill_comparator(int a,const City*c) : a1{a}, city{c} {}

		bool operator()(const int l1, const int l2)
		{
			return city->get_time_to(a1, l1) < city->get_time_to(a1, l2);
		}
	};
}

void find_path_between_requests(const Solution* solution, int a1, int a2, int *path, int& len, int& time)
{
	const City* c = solution->get_city();
	if (a1 != START_ACTION_INDEX && a1 != END_ACTION_INDEX)
	INBOUNDS(c->first_request_index, a1, c->num_actions);
	if (a2 != START_ACTION_INDEX && a2 != END_ACTION_INDEX)
	INBOUNDS(c->first_request_index, a2, c->num_actions);

	time = INT_MAX;

	int incoming_truck_size = c->get_action(a1).exit_state & TRUCK_SIZE_MASK;
	int outgoing_truck_size = c->get_action(a2).entr_state & TRUCK_SIZE_MASK;
	bool starts_full = state_is_full(c->get_action(a1).exit_state);

	if (incoming_truck_size == outgoing_truck_size)
	{
		if (!starts_full)
		{
			len = 0;
			time = c->get_time_to(a1, a2);
			return;
		}


		int loffset = get_matching_landfill_index((dumpster_size) (c->get_action(a1).exit_state & TRUCK_SIZE_MASK));
		// need to visit landfill
		for (int l = 0; l < c->num_landfills; l++)
		{
			int la = c->first_landfill_index + l * NUM_ACTIONS_PER_FILL + loffset;
			int tmp_time = c->get_time_to(a1, la) + c->get_time_to(la, a2);
			if (tmp_time >= time)
			{
				continue;
			}

			path[0] = la;
			len = 1;
			time = tmp_time;
		}
		return;
	}

	int soffset = get_matching_staging_area_index(
			(dumpster_size) (c->get_action(a1).exit_state & TRUCK_SIZE_MASK),
			(dumpster_size) (c->get_action(a2).entr_state & TRUCK_SIZE_MASK));

	if (!starts_full)
	{
		// just need to get the right staging area
		for (int s = 0; s < c->num_stagingareas; s++)
		{
			int sa = c->first_stagingarea_index + NUM_ACTIONS_PER_YARD * s + soffset;
			int tmp_time = c->get_time_to(a1, sa) + c->get_time_to(sa, a2);
			if (tmp_time >= time)
			{
				continue;
			}
			time = tmp_time;
			path[0] = sa;
			len = 1;
		}

		return;
	}

	int loffset = get_matching_landfill_index((dumpster_size) (c->get_action(a1).exit_state & TRUCK_SIZE_MASK));
	int *fills = new int[c->num_landfills];
	for (int l = 0; l < c->num_landfills; l++)
	{
		fills[l] = c->first_landfill_index + l * NUM_ACTIONS_PER_FILL + loffset;
	}

	std::sort(&fills[0], &fills[c->num_landfills], landfill_comparator{a1, c});
	for (int i = 0; i < c->num_landfills; i++)
	{
		int la = fills[i];
		const int t1 = c->get_time_to(a1, la);
		if (t1 >= time)
		{
			continue;
		}

//		if (t1 + c->get_time_to(la, a2) >= time)
//		{
//			continue; ?
//		}

		for (int s = 0; s < c->num_stagingareas; s++)
		{
			int sa = c->first_stagingarea_index + NUM_ACTIONS_PER_YARD * s + soffset;
			int tmp_time = t1 + c->get_time_to(la, sa) + c->get_time_to(sa, a2);
			if (tmp_time >= time)
			{
				continue;
			}

			time = tmp_time;
			path[0] = la;
			path[1] = sa;
			len = 2;
		}
	}

	delete[] fills;

	return;
}
