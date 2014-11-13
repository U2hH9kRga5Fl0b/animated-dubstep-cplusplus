/*
 * pairing_info.cpp
 *
 *  Created on: Nov 5, 2014
 *      Author: thallock
 */

#include "insight/pairing_info.h"

#include <list>
#include <algorithm>

namespace
{
	struct b4_dpt_cmp
	{
		const City* city; int loc;
		b4_dpt_cmp(const City* c, int i) : city{c}, loc{i} {}
		bool operator()(const int d1, const int d2) const
		{
			// This doesn't take the landfill wait times into account!!!
			return city->durations.at(city->yards.at(d1).location,loc) <
					city->durations.at(city->yards.at(d2).location,loc);

		}
	};
	struct aftr_dpt_cmp
	{
		const City* city; int loc;
		aftr_dpt_cmp(const City* c, int i) : city{c}, loc{i} {}
		bool operator()(const int d1, const int d2) const
		{
			// This doesn't take the landfill wait times into account!!!
			return city->durations.at(loc, city->yards.at(d1).location) <
					city->durations.at(loc, city->yards.at(d2).location);
		}
	};
}


pairing_info::pairing_info(const City* city_) :
		city{city_},
		num_staging_areas{(int) city->yards.size()},
		depot_2_depot{num_staging_areas, num_staging_areas}
{
	std::list<int> ps;
	std::list<int> ds;

	for (int i = 0; i < city->num_requests; i++)
	{
		const Action& action = city->get_action(i + city->first_request_index);
		if (state_is_full(action.exit_state))
			ps.push_back(i + city->first_request_index);
		else
			ds.push_back(i + city->first_request_index);
	}

	int i = 0;
	pickup_lens = ps.size();
	pickup_actions = new int[pickup_lens];
	for (auto it = ps.begin(); it != ps.end(); ++it, i++)
		pickup_actions[i] = *it;

	i = 0;
	deliver_lens = ds.size();
	deliver_actions = new int[deliver_lens];
	for (auto it = ds.begin(); it != ds.end(); ++it, i++)
		deliver_actions[i] = *it;


	std::sort(&pickup_actions[0], &pickup_actions[pickup_lens], [city_](const int p1, const int p2)
	{	return (city_->get_action(p1).exit_state & TRUCK_SIZE_MASK) < (city_->get_action(p2).exit_state & TRUCK_SIZE_MASK);});
	std::sort(&deliver_actions[0], &deliver_actions[deliver_lens], [city_](const int d1, const int d2)
	{	return (city_->get_action(d1).entr_state & TRUCK_SIZE_MASK) < (city_->get_action(d2).entr_state & TRUCK_SIZE_MASK);});

//	for (int i = 0; i < deliver_lens; i++)
//		for (int j = 0; j <pickup_lens; j++)
//			ret->d2p.at(i, j) = city->get_time_to(-1, ret->deliver_actions[i], ret->pickup_actions[j]);





	for (int i = 0; i < 4; i++)
	{
		deliver_sizes[i].begin = deliver_lens;
		pickup_sizes[i].begin = pickup_lens;

		deliver_sizes[i].end = 0;
		pickup_sizes[i].end = 0;
	}

	bool found[4];
	found[0] = found[1] = found[2] = found[3] = false;
	for (int j = 0; j < pickup_lens; j++)
	{
		int size = d2i(city->get_action(pickup_actions[j]).exit_state & TRUCK_SIZE_MASK);
		found[size] = true;
		if (pickup_sizes[size].begin > j)
		{
			pickup_sizes[size].begin = j;
		}
		if (pickup_sizes[size].end < j+1)
		{
			pickup_sizes[size].end = j+1;
		}
	}
	for (int i = 0; i < 4; i++)
	{
		if (found[i])
			continue;

		pickup_sizes[i].begin = pickup_lens;
		pickup_sizes[i].end = pickup_lens;
	}

	found[0] = found[1] = found[2] = found[3] = false;
	for (int j = 0; j < deliver_lens; j++)
	{
		int size = d2i(city->get_action(deliver_actions[j]).entr_state & TRUCK_SIZE_MASK);
		found[size] = true;
		if (deliver_sizes[size].begin > j)
		{
			deliver_sizes[size].begin = j;
		}
		if (deliver_sizes[size].end < j+1)
		{
			deliver_sizes[size].end = j+1;
		}
	}

	for (int i = 0; i < 4; i++)
	{
		if (found[i])
			continue;

		deliver_sizes[i].begin = deliver_lens;
		deliver_sizes[i].end = deliver_lens;
	}

	if (DEBUG)
	{

		for (int i = 0; i < 4; i++)
			log() << pickup_sizes[i].begin << "-" << pickup_sizes[i].end << " ";
		log () << std::endl;
		for (int i = 0; i < 4; i++)
			log() << deliver_sizes[i].begin << "-" << deliver_sizes[i].end << " ";
		log () << std::endl;
	}

	closest_delivers = new int*[deliver_lens];
	for (int i = 0; i < deliver_lens; i++)
	{
		closest_delivers[i] = new int[num_staging_areas];
		for (int j = 0; j < num_staging_areas; j++)
			closest_delivers[i][j] = j;
		int loc = city->get_action(deliver_actions[i]).location;
		std::sort(&closest_delivers[i][0], &closest_delivers[i][0] + num_staging_areas, b4_dpt_cmp { city, loc });
	}

	closest_pickups = new int*[pickup_lens];
	for (int i = 0; i < pickup_lens; i++)
	{
		closest_pickups[i] = new int[num_staging_areas];
		for (int j = 0; j < num_staging_areas; j++)
			closest_pickups[i][j] = j;
		int loc = city->get_action(pickup_actions[i]).location;
		std::sort(&closest_pickups[i][0], &closest_pickups[i][0] + num_staging_areas, aftr_dpt_cmp { city, loc });
	}

	for (int i = 0; i < num_staging_areas; i++)
		for (int j = 0; j < num_staging_areas; j++)
			depot_2_depot.at(i, j) = city->durations.at(city->yards.at(i).location, city->yards.at(j).location);
}

pairing_info::~pairing_info()
{
	delete[] deliver_actions;
	for (int i = 0; i < deliver_lens; i++)
		delete[] closest_delivers[i];
	delete[] closest_delivers;

	delete[] pickup_actions;
	for (int i = 0; i < pickup_lens; i++)
		delete[] closest_pickups[i];
	delete[] closest_pickups;
}



int pairing_info::get_nth_closest_deliver_depot(int deliver_index, int n) const
{
	INBOUNDS(0, deliver_index, deliver_lens);
	INBOUNDS(0, n, num_staging_areas);
	return closest_delivers[deliver_index][n];
}

int pairing_info::get_nth_closest_pickup_depot(int pickup_index, int n) const
{
	INBOUNDS(0, pickup_index, pickup_lens);
	INBOUNDS(0, n, num_staging_areas);
	return closest_pickups[pickup_index][n];
}

#if 0

pairing_info* new_pairing_info(const City* city)
{

	std::sort(&ret->pickup_actions[0], &ret->pickup_actions[ret->num_pickups], [city](const int p1, const int p2)
	{	return (city->get_action(p1).exit_state & TRUCK_SIZE_MASK) < (city->get_action(p2).exit_state & TRUCK_SIZE_MASK);});
			std::sort(&ret->deliver_actions[0], &ret->deliver_actions[ret->num_delivers], [city](const int d1, const int d2)
			{return (city->get_action(d1).entr_state & TRUCK_SIZE_MASK) < (city->get_action(d2).entr_state & TRUCK_SIZE_MASK); });


	for (int i = 0; i < ret->num_delivers; i++)
		for (int j = 0; j < ret->num_pickups; j++)
			ret->d2p.at(i, j) = city->get_time_to(-1, ret->deliver_actions[i], ret->pickup_actions[j]);

	dumpster_size sizes[4] = { six, nine, twelve, sixteen };
	for (int i = 0; i < 4; i++)
	{
		ret->first_deliver_of_size[i] = ret->num_delivers;
		ret->first_pickup_of_size[i] = ret->num_pickups;

		for (int j = (i == 0 ? 0 : ret->first_deliver_of_size[i - 1]); j < ret->num_delivers; j++)
			if (sizes[i] == (city->get_action(ret->deliver_actions[j]).entr_state & TRUCK_SIZE_MASK))
			{
				ret->first_deliver_of_size[i] = j;
				break;
			}
		for (int j = (i == 0 ? 0 : ret->first_pickup_of_size[i - 1]); j < ret->num_pickups; j++)
			if (sizes[i] == (city->get_action(ret->pickup_actions[j]).exit_state & TRUCK_SIZE_MASK))
			{
				ret->first_pickup_of_size[i] = j;
				break;
			}
	}

	for (int i = 0; i < ret->num_delivers; i++)
	{
		for (int j = 0; j < city->num_stagingareas; j++)
			ret->deliver_depots.at(i, j) = j;
		std::sort(&ret->deliver_depots.at(i, 0), &ret->deliver_depots.at(i, 0) + city->num_stagingareas,
				b4_dpt_cmp{city, city->get_action(ret->deliver_actions[i]).location});
	}
	for (int i = 0; i < ret->num_pickups; i++)
	{
		for (int j = 0; j < city->num_stagingareas; j++)
			ret->pickup_depots.at(i, j) = j;
		std::sort(&ret->pickup_depots.at(i, 0), &ret->pickup_depots.at(i, 0) + city->num_stagingareas,
				aftr_dpt_cmp{city, city->get_action(ret->pickup_actions[i]).location});
	}


	std::cout << "yards: " << std::endl;
	for (int i = 0; i < city->num_stagingareas; i++)
	{
		std::cout << city->coords[city->yards.at(i).location] << std::endl;
	}

	std::cout << "fills: " << std::endl;
	for (int i = 0; i < city->num_landfills; i++)
	{
		std::cout << city->coords[city->get_action(i * NUM_ACTIONS_PER_FILL).location] << std::endl;
	}

	return ret;
}
#endif
