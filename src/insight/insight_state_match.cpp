/*
 * insight_state_match.cpp
 *
 *  Created on: Nov 11, 2014
 *      Author: thallock
 */

#include "insight/insight_state.h"
#include "insight/insight_viewer.h"

#include "c/hungarian.h"


std::set<int> AVOID_NO_ACTIONS;


#if 1

void insight_state::fast_match(const std::set<int>& avoid_actions)
{
	int number_of_deliveries = info->deliver_lens;
	int number_of_pickups = info->pickup_lens;

	if (number_of_deliveries == 0 || number_of_pickups == 0)
		return;

	intarray cost_matrix { number_of_deliveries, 2 * number_of_pickups };
	if (cost_matrix.rows() > cost_matrix.cols())
	{
		trap();
	}

	auto avoid_end = avoid_actions.end();

	for (int i = 0; i < number_of_deliveries; i++)
	{
		int da = info->deliver_actions[i];
		int dloc = info->city->get_action(da).location;
		int dd = false ? deliver_depots[i] : info->get_nth_closest_deliver_depot(i, 0);
		int ddloc = info->city->yards.at(dd).location;

		if (avoid_actions.find(da) != avoid_end)
		{
			for (int j = 0; j < number_of_pickups; j++)
			{
				cost_matrix.at(i, j) = 1;
				cost_matrix.at(i, j + number_of_pickups) = 0;
			}
			continue;
		}

		int min_cost = 0;

		for (int j = 0; j < number_of_pickups; j++)
		{
			int pick = info->pickup_actions[j];
			int ploc = info->city->get_action(pick).location;
			int pd = false ? pickup_depots[j] : info->get_nth_closest_pickup_depot(j, 0);
			int pdloc = info->city->yards.at(pd).location;

			int paired_distance = info->city->durations.at(dloc, ploc);
			int separt_distance = info->city->durations.at(dloc, ddloc) + info->city->durations.at(ploc, pdloc);
			int delta = avoid_actions.find(pick) != avoid_end ? INT_MAX : paired_distance - separt_distance;

			cost_matrix.at(i, j) = delta;
			// this line needs to be fixed...
			cost_matrix.at(i, number_of_pickups + j) = 0;
			min_cost = std::min(delta, min_cost);
		}

		const int ncools = cost_matrix.cols();
		for (int j = 0; j < ncools; j++)
		{
			cost_matrix.at(i, j) += -min_cost;
		}
	}

	log () << cost_matrix << std::endl;

	int *assignment = solve_assignment_problem(cost_matrix);
	for (int i = 0; i < number_of_deliveries; i++)
	{
		if (assignment[i] < number_of_pickups
				&& avoid_actions.find(info->deliver_actions[i]) == avoid_end
				&& avoid_actions.find(info->pickup_actions[assignment[i]]) == avoid_end)
			match(i,  assignment[i]);
		else
			match(i, -1);
	}
	delete[] assignment;

	void *video = create_video();
	show_insight("foobar", this, video);
	destroy_video(video);
}

int get_time_to(const City* city, int prev, int hub, int driver=-1)
{
	if (state_is_full(city->get_action(prev, driver).exit_state))
	{
		int sum = 0;
		int lindex = city->get_landfill_index(hub,
				get_state_size(city->get_action(prev, driver).exit_state));
		sum += city->get_time_to(driver, prev, lindex);

		log() << city->get_time_to(driver, prev, lindex) << ' ';

		int sindex = city->get_staging_area_index(hub,
				get_state_size(city->get_action(prev, driver).exit_state),
				(dumpster_size) 0);
		sum += city->get_time_to(driver, lindex, sindex);


		log() << city->get_time_to(driver, lindex, sindex) << ' ';

		return sum;
	}
	else
	{
		truck_state st = city->get_action(prev, driver).exit_state;
		int sindex = city->get_staging_area_index(hub,
				get_state_size(st),
				get_state_size(st) == 0 ? (dumpster_size) 6 : (dumpster_size) 0);

		log() << city->get_time_to(driver, prev, sindex) << ' ';

		return city->get_time_to(driver, prev, sindex);
	}
}
int get_time_from(const City* city, int hub, int next, int driver = -1)
{
	truck_state st = city->get_action(next, driver).entr_state;
	int sindex = city->get_staging_area_index(hub,
			get_state_size(st) == 0 ? (dumpster_size) 6 : (dumpster_size) 0, get_state_size(st));
	return city->get_time_to(driver, sindex, next);
}

int insight_state::get_cost() const
{
	std::set<int> rem_pickups;

	int sum = 0;
	for (int i = 0; i < info->deliver_lens; i++)
	{
		sum += get_time_from(info->city, deliver_depots[i], info->deliver_actions[i]);
		log() << get_time_from(info->city, deliver_depots[i], info->deliver_actions[i]) << ' ';

		int pickup = delivers_to_pickups[i];
		if (pickup >= 0)
		{
			sum += info->city->get_time_to(-1, info->deliver_actions[i], info->pickup_actions[pickup]);
			log() << info->city->get_time_to(-1, info->deliver_actions[i], info->pickup_actions[pickup]) << ' ';

			sum += get_time_to(info->city,  info->pickup_actions[pickup], pickup_depots[pickup]);
		}
		else
		{
			sum += get_time_to(info->city, info->deliver_actions[i], unmatched_delivers[i]);
		}

		log () << std::endl;
	}

	for (int i = 0; i < info->pickup_lens; i++)
	{
		int pickup_depot = unmatched_pickups[i];
		if (pickup_depot < 0)
		{
			continue;
		}

		sum += get_time_from(info->city, pickup_depot, info->pickup_actions[i]);
		log() << get_time_from(info->city, pickup_depot, info->pickup_actions[i]);
		sum += get_time_to(info->city, pickup_depots[i], pickup_depots[i]);
		log () << std::endl;
	}

	for (int i = 0; i < info->city->num_trucks; i++)
	{
		if (embark[i].path[1] >= 0) trap();
		if (embark[i].path[0] < 0)
		{
			sum += get_time_to(info->city, embark[i].begin, embark[i].end, i);
		}
		else
		{
			sum += info->city->get_time_to(i, embark[i].begin,   embark[i].path[0]);
			sum += get_time_to(info->city, embark[i].path[0], embark[i].end, i);
		}
		if (embark[i].end < 0)
		{
			// straight from begin to end...
			continue;
		}

		if (embark[i].path[1] >= 0) trap();
		if (embark[i].path[0] < 0)
		{
			sum += get_time_from(info->city, embark[i].begin, embark[i].end, i);
		}
		else
		{
			sum += get_time_from(info->city, embark[i].begin,   embark[i].path[0], i);
			sum += info->city->get_time_to(i, embark[i].path[0], embark[i].end);
		}
	}

	return sum;
}

insight_state& insight_state::operator =(const insight_state& other)
{
	if (info != other.info)
	{
		trap();
	}
	for (int i = 0; i < info->deliver_lens; i++)
	{
		deliver_depots[i] = other.deliver_depots[i];
		delivers_to_pickups[i] = other.delivers_to_pickups[i];
		unmatched_delivers[i] = other.unmatched_delivers[i];
	}
	for (int i = 0; i < info->pickup_lens; i++)
	{
		pickup_depots[i] = other.pickup_depots[i];
		unmatched_pickups[i] = other.unmatched_pickups[i];
	}
	for (int i = 0; i < info->city->num_trucks; i++)
	{
		embark[i] = other.embark[i];
		finish[i] = other.finish[i];
	}
	return *this;
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

