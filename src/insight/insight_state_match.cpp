/*
 * insight_state_match.cpp
 *
 *  Created on: Nov 11, 2014
 *      Author: thallock
 */

#include "insight/insight_state.h"
#include "insight/insight_viewer.h"

#include "c/hungarian.h"


#if 1

void insight_state::fast_match()
{
	// for each dumpster size
	for (int s = 0; s < 4; s++)
	{
		int number_of_deliveries = info->deliver_lens[s];
		int number_of_pickups = info->pickup_lens[s];

		if (number_of_deliveries == 0 || number_of_pickups == 0)
			continue;

		intarray cost_matrix { number_of_deliveries, 2 * number_of_pickups };
		if (cost_matrix.rows() > cost_matrix.cols())
		{
			trap();
		}

		for (int i = 0; i < number_of_deliveries; i++)
		{
			int da = info->deliver_actions[s][i];
			int dloc = info->city->get_action(da).location;
			int dd = false ? deliver_depots[s][i] : info->get_nth_closest_deliver_depot(s, i, 0);
			int ddloc = info->city->yards.at(dd).location;

			for (int j = 0; j < number_of_pickups; j++)
			{
				int pick = info->pickup_actions[s][j];
				int ploc = info->city->get_action(pick).location;
				int pd = false ? pickup_depots[s][j] : info->get_nth_closest_pickup_depot(s, j, 0);
				int pdloc = info->city->yards.at(pd).location;

				int paired_distance = info->city->durations.at(dloc, ploc);
				int separt_distance = info->city->durations.at(dloc, ddloc) + info->city->durations.at(ploc, pdloc);

				cost_matrix.at(i, j) = paired_distance;
				cost_matrix.at(i, number_of_pickups + j) = separt_distance;
			}
		}

		int *assignment = solve_assignment_problem(cost_matrix);
		for (int i = 0; i < number_of_deliveries; i++)
			if (assignment[i] < number_of_pickups)
				delivers_to_pickups[s][i] = assignment[i];
			else
				delivers_to_pickups[s][i] = -1;
		delete[] assignment;
	}

	void *video = create_video();
	show_insight("foobar", this, video);
	destroy_video(video);
}

#elif 1

void insight_state::fast_match()
{
	bool *already_pickedup = new bool[info->num_pickups];
	bool *already_delivered = new bool[info->num_delivers];

	void *video = create_video();

	for (int i = 0; i < info->num_pickups; i++)
		already_pickedup[i] = false;
	for (int i = 0; i < info->num_delivers; i++)
		already_delivered[i] = false;
	for (int i = 0; i < info->num_delivers; i++)
		delivers_to_pickups[i] = -1;

	// pick the shortest first
	for (int i = 0; i < 4; i++)
	{
		for (int i = 0; i < 4; i++)
		{
			int dbegin = info->first_deliver_of_size[i];
			int dend = i == 3 ? info->num_delivers : info->first_deliver_of_size[i + 1];
			int pbegin = info->first_pickup_of_size[i];
			int pend = i == 3 ? info->num_pickups : info->first_pickup_of_size[i + 1];

			while (true)
			{
				int di, pi, imp;

				di = pi = -1;
				imp = INT_MAX;

				for (int d = dbegin; d < dend; d++)
				{
					if (already_delivered[d])
					{
						continue;
					}
					int da = info->deliver_actions[d];
					int dloc = info->city->get_action(da).location;
					int dd = false ? deliver_depots[d] : info->deliver_depots.at(d, 0);
					int ddloc = info->city->yards.at(dd).location;
					for (int p = pbegin; p < pend; p++)
					{
						if (already_pickedup[p])
						{
							continue;
						}
						int pick = info->pickup_actions[p];
						int ploc = info->city->get_action(pick).location;
						int pd = false ? pickup_depots[p] : info->pickup_depots.at(p, 0);
						int pdloc = info->city->yards.at(pd).location;

						int paired_distance = info->city->durations.at(dloc, ploc);
						int separt_distance = info->city->durations.at(dloc, ddloc) + info->city->durations.at(ploc, pdloc);

						int nextpairing_cost = paired_distance - separt_distance;

						if (nextpairing_cost >= imp)
						{
							continue;
						}
						imp = nextpairing_cost;
						di = d;
						pi = p;
					}
				}

				if (imp >= 0)
				{
					break;
				}
				delivers_to_pickups[di] = pi;
				already_delivered[di] = true;
				already_pickedup[pi] = true;
				show_insight("foobar", this, video);
			}
		}
	}

	show_insight("foobar", this, video, "done");

	destroy_video(video);
	delete[] already_pickedup;
	delete[] already_delivered;
}

#else

This is an alternative fast_match that is even faster, less quality, and not debugged.
However, it used to compile when I wrote it! :)

		int starts[4] = {info->first_deliver_of_size[0],
				info->first_deliver_of_size[1],
				info->first_deliver_of_size[2],
				info->first_deliver_of_size[3]};
		int ends[4] = { info->first_deliver_of_size[1],
				info->first_deliver_of_size[2],
				info->first_deliver_of_size[3],
				info->num_delivers};

		bool *already_pickedup = new bool[info->num_pickups];
		for (int i = 0; i < info->num_pickups; i++)
			already_pickedup[i] = false;

		for (int i = 0; i < 4; i++)
		for (int d = starts[i]; d < ends[i]; d++)
		{
			int deliver = info->deliver_actions[d];
			int pickupIdx = -1;
			int cost = INT_MAX;

			int pstart = info->first_pickup_of_size[i];
			int pend = i == 3 ? info->num_pickups : info->first_pickup_of_size[i+1];
			for (int j = pstart; j < pend; j++)
			{
				if (already_pickedup[j])
					continue;
				int a = info->d2p.at(deliver, info->pickup_actions[j]);
				if (a < cost)
				{
					cost = a;
					pickupIdx = j;
				}
			}

			if (pickupIdx < 0)
			{
				delivers_to_pickups[d] = -1;
				continue;
			}

			int pickup = info->pickup_actions[pickupIdx];
			if (cost >= info->city->get_time_to(deliver, deliver_depots[i]) + info->city->get_time_to(pickup, pickup_depots[pickup]))
			{
				delivers_to_pickups[d] = -1;
				continue;
			}

			delivers_to_pickups[d] = pickup;
			already_pickedup[pickupIdx] = true;
		}

		delete[] already_pickedup;

#endif

