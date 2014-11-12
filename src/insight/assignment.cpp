/*
 * assignment.cpp
 *
 *  Created on: Nov 5, 2014
 *      Author: thallock
 */

#include "insight/assignment.h"

#include "main/global.h"

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
			for (int s = 0; s < 4; s++)
			{
				int num_picks = state->info->pickup_lens[s];
				int num_dels = state->info->deliver_lens[s];

				bool* already_pickedup = new bool[num_picks];
				for (int i = 0; i < num_picks; i++)
					already_pickedup[i] = false;

				for (int i = 0; i < num_dels; i++)
				{
					int pickup_index = state->delivers_to_pickups[s][i];
					if (pickup_index < 0)
					{
						int begin_yard = state->deliver_depots[s][i];
						int end_yard = state->deliver_depots[s][i]; // TODO: this is not going to be true in general!!

						interhub interh { begin_yard, end_yard };
						interh.path[0] = state->info->deliver_actions[s][i];

						paths_from_hubs[begin_yard].push_back(interh);
					}
					else
					{
						int begin_yard = state->deliver_depots[s][i];
						int pickup = state->info->pickup_actions[s][pickup_index];
						int end_yard = state->pickup_depots[s][pickup_index];

						interhub interh { begin_yard, end_yard };
						interh.path[0] = state->info->deliver_actions[s][i];
						interh.path[1] = pickup;

						paths_from_hubs[begin_yard].push_back(interh);
						already_pickedup[pickup_index] = true;
					}
				}

				for (int i = 0; i < num_picks; i++)
				{
					if (already_pickedup[i])
					{
						continue;
					}
					int begin_yard = state->pickup_depots[s][i];
					int end_yard = begin_yard; // TODO: this is not going to be true in general!!

					interhub interh { begin_yard, end_yard };
					interh.path[0] = state->info->pickup_actions[s][i];
					paths_from_hubs[begin_yard].push_back(interh);
				}

				delete[] already_pickedup;
			}
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

		bool options(int hub) const
		{
			return paths_from_hubs[hub].size();
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
	{
		int start_location = city->begin_actions[i].location;
		int curmin = INT_MAX;
		for (int j = 0; j < city->num_stagingareas; j++)
		{
			int staging_area_location = city->yards.at(j).location;
			int alt = city->durations.at(start_location, staging_area_location);
			if (alt >= curmin)
				continue;
			curmin = alt;
			current_yards[i] = j;
		}
	}

	Solution* ret_val = new Solution{city, 3 * state->info->city->num_actions};

	int size = collection.size();
	while (size --> 0)
	{
		int ntdriver = -1;
		int mindrivertime = INT_MAX;

		int nmdriver = -1;
		int minmdrivertime = INT_MAX;

		for (int i = 0; i < numtrucks; i++)
		{
			int t = ret_val->get_time_for_driver(i);
			if (collection.options(current_yards[i]))
			{
				if (t < minmdrivertime)
				{
					nmdriver = i;
					minmdrivertime = t;
				}
			}
			if (t < mindrivertime)
			{
				ntdriver = i;
				mindrivertime = t;
			}
		}

		int ndriver = nmdriver < 0 ? ntdriver : nmdriver;

		int clength = ret_val->get_number_of_stops(ndriver);

		int chub = current_yards[ndriver];
		interhub subpath = collection.get_next_subpath(chub);

		int faction = subpath.path[0];
		int saction = subpath.path[1];
		int laction = faction;

		dumpster_size csize =   (dumpster_size) (city->get_action(ret_val->get_action_index(ndriver, clength-1), ndriver).exit_state & TRUCK_SIZE_MASK);
		dumpster_size in_size = (dumpster_size) (city->get_action(faction).entr_state & TRUCK_SIZE_MASK);

		if (csize == subpath.end)
		{
			log() << "matched" << std::endl;
		}
		else
		{
			log() << "did not match" << std::endl;
		}

		log() << collection << std::endl;
		log() << ndriver << " performing " << subpath << std::endl;
		for (int i = 0; i < numtrucks; i++)
			log() << current_yards[i] << " ";
		log() << std::endl;


		if (csize != in_size)
		{
			int stage_action = city->get_staging_area_index(chub, csize, in_size);
			ret_val->append(ndriver, clength++, stage_action);

			log() << *ret_val << std::endl;

			viewer.show("building", ret_val);
			viewer.pause(20);
		}
		ret_val->append(ndriver, clength++, faction);
		if (saction >= 0)
		{
			ret_val->append(ndriver, clength++, saction);

			viewer.show("building", ret_val);
			viewer.pause(20);

			laction = saction;
		}

		if (state_is_full(city->get_action(laction).exit_state))
		{
			dumpster_size outsize = (dumpster_size) (city->get_action(laction).exit_state & TRUCK_SIZE_MASK);
			ret_val->append(ndriver, clength++, city->get_landfill_index(subpath.end, outsize));
		}

		current_yards[ndriver] = subpath.end;

		viewer.show("building", ret_val);
		viewer.pause(20);
	}

	for (int i = 0; i < numtrucks; i++)
	{
		int len = ret_val->get_number_of_stops(i);
		if (len == 0)
		{
			continue;
		}
		const Action& act = city->get_action(ret_val->get_action_index(i, len-1), i);

		if (act.exit_state == TRUCK_STATE_NONE)
		{
			continue;
		}

		// We shouldn't just use the closest yard...
		int end_location = city->final_actions[i].location;
		int endyard = -1300;
		int curmin = INT_MAX;
		for (int j = 0; j < city->num_stagingareas; j++)
		{
			int staging_area_location = city->yards.at(j).location;
			int alt = city->durations.at(end_location, staging_area_location);
			if (alt >= curmin)
				continue;
			endyard = j;
			curmin = alt;
		}

		if (state_is_full(act.exit_state))
		{
			dumpster_size outsize = (dumpster_size) (act.exit_state & TRUCK_SIZE_MASK);
			ret_val->append(i, len++, city->get_landfill_index(endyard, outsize));
		}

		ret_val->append(i, len, city->get_staging_area_index(
				endyard,
				(dumpster_size) (act.exit_state & TRUCK_SIZE_MASK), none));
	}

	log() << collection.size() << std::endl;

	ret_val->ensure_valid();
	log() << *ret_val << std::endl;
	ret_val->human_readable(log());

	delete[] current_yards;

	return ret_val;
}
