/*
 * assignment.cpp
 *
 *  Created on: Nov 5, 2014
 *      Author: thallock
 */

#include "insight/assignment.h"

namespace
{
	class subpath_collection
	{
	public:
		subpath_collection(insight_state* state) :
			depot_to_depot{&state->info->depot_2_depot},
			num_yards{state->info->num_staging_areas},
			paths_from_hubs{new std::list<interhub>[num_yards]}
		{
			bool* already_pickedup = new bool[state->info->num_pickups];
			for (int i = 0; i < state->info->num_pickups; i++)
				already_pickedup[i] = false;

			for (int i = 0; i < state->info->num_delivers; i++)
			{
				int pickup_index = state->delivers_to_pickups[i];
				if (pickup_index < 0)
				{
					int begin_yard = state->deliver_depots[i];
					int end_yard = state->deliver_depots[i]; // TODO: this is not going to be true in general!!

					interhub interh { begin_yard, end_yard };
					interh.path[0] = state->info->deliver_actions[i];

					paths_from_hubs[begin_yard].push_back(interh);
				}
				else
				{
					int begin_yard = state->deliver_depots[i];
					int pickup = state->pickup_depots[pickup_index];
					int end_yard = state->pickup_depots[pickup_index];

					interhub interh { begin_yard, end_yard };
					interh.path[0] = state->info->deliver_actions[i];
					interh.path[1] = pickup;

					paths_from_hubs[begin_yard].push_back(interh);
					already_pickedup[pickup_index] = true;
				}
			}

			for (int i = 0; i < state->info->num_pickups; i++)
			{
				if (already_pickedup[i])
				{
					continue;
				}
				int begin_yard = state->pickup_depots[i];
				int end_yard = begin_yard; // TODO: this is not going to be true in general!!

				interhub interh {begin_yard, end_yard};
				interh.path[0] = state->info->pickup_actions[i];
				paths_from_hubs[begin_yard].push_back(interh);
			}

			delete[] already_pickedup;
		}

		int size() const
		{
			int ret = 0;
			for (int i = 0; i < num_yards; i++)
			{
				ret += paths_from_hubs[i].size();
			}
			return ret;
		}

		interhub get_next_subpath(int from_yard)
		{
			if (paths_from_hubs[from_yard].size() == 0)
			{
				// uh oh. time to get the closest one.

				int time_to_closest = INT_MAX;
				int closest = -1;
				for (int i = 0; i < num_yards; i++)
				{
					if (paths_from_hubs[i].size() == 0)
					{
						continue;
					}
					int di = depot_to_depot->at(from_yard, i);
					if (di >= time_to_closest)
					{
						continue;
					}
					closest = i;
					time_to_closest = di;
				}

				return get_next_subpath(closest);
			}

			auto it = paths_from_hubs[from_yard].begin();
			interhub retval = *it;
			paths_from_hubs[from_yard].erase(it);
			return retval;
		}

		friend std::ostream& operator<<(std::ostream& out, const subpath_collection& c)
		{
			for (int i = 0; i < c.num_yards; i++)
			{
				out << "\t" << i << ":\n";
				auto end = c.paths_from_hubs[i].end();
				for (auto it = c.paths_from_hubs[i].begin(); it != end; ++it)
				{
					out << "\t\t" << *it << '\n';
				}
			}
			return out << std::endl;
		}

		const intarray *depot_to_depot;
		int num_yards;
		std::list<interhub> *paths_from_hubs;
	};
}


Solution* assign_insight(insight_state* state)
{
	subpath_collection collection{state};

	log() << "this created subpaths:\n" << collection << std::endl;

	const City* city = state->info->city;
	const int numtrucks = city->num_trucks;

	int *current_yards = new int[numtrucks];
	for (int i = 0; i < numtrucks; i++)
		current_yards[i] = city->start_yard;

	Solution* ret_val = new Solution{state->info->city, 3 * state->info->city->num_actions};

	int size = collection.size();
	while (size --> 0)
	{
		int ndriver = -1;
		int mindrivertime = INT_MAX;
		for (int i = 0; i < numtrucks; i++)
		{
			int t = ret_val->get_time_for_driver(i);
			if (t >= mindrivertime)
			{
				continue;
			}
			ndriver = i;
			mindrivertime = t;
		}
		int clength = ret_val->get_number_of_stops(ndriver);

		int chub = current_yards[ndriver];
		interhub subpath = collection.get_next_subpath(chub);

		int faction = subpath.path[0];
		int saction = subpath.path[1];


		dumpster_size csize = (dumpster_size) (city->get_action(ret_val->get_action_index(ndriver, clength)).exit_state & TRUCK_SIZE_MASK);
		dumpster_size in_size = (dumpster_size) (city->get_action(faction).entr_state & TRUCK_SIZE_MASK);

		if (csize != in_size)
		{
			int stage_action = city->get_staging_area_index(chub, csize, in_size);
			ret_val->append(ndriver, clength++, stage_action);
		}
		ret_val->append(ndriver, clength++, faction);
		if (saction >= 0)
		{
			ret_val->append(ndriver, clength++, saction);
			dumpster_size outsize = (dumpster_size) (city->get_action(saction).exit_state & TRUCK_SIZE_MASK);
			ret_val->append(ndriver, clength++, city->get_landfill_index(chub, outsize));
		}

		current_yards[ndriver] = subpath.end;
	}

	std::cout << collection.size() << std::endl;

	ret_val->ensure_valid();
	std::cout << *ret_val << std::endl;
	ret_val->human_readable(std::cout);

	delete[] current_yards;

	return ret_val;
}
