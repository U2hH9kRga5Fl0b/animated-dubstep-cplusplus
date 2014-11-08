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

pairing_info::pairing_info(const City* city_, int d, int p, int npts) :
			city{city_},
			num_staging_areas{npts},
			num_pickups{p},
			pickup_actions{new int[p]},
			pickup_depots {p , npts},
			num_delivers{d},
			deliver_actions{new int[d]},
			deliver_depots {d , npts},
			depot_2_depot{npts, npts},
			d2p{d, p} {}

pairing_info::~pairing_info()
{
	delete[] pickup_actions;
	delete[] deliver_actions;
}

pairing_info* new_pairing_info(const City* city)
{
	std::list<int> ps;
	std::list<int> ds;
	for (int i = 0; i < city->num_requests; i++)
		if (state_is_full(city->get_action(i + city->first_request_index).exit_state))
			ps.push_back(i + city->first_request_index);
		else
			ds.push_back(i + city->first_request_index);

	pairing_info* ret = new pairing_info{city, (int) ds.size(), (int) ps.size(), city->num_stagingareas};

	{	auto end = ps.end(); int i = 0;
		for (auto it = ps.begin(); it != end; ++it, i++)
			ret->pickup_actions[i] = *it;
	} {	auto end = ds.end(); int i = 0;
		for (auto it = ds.begin(); it != end; ++it, i++)
			ret->deliver_actions[i] = *it;
	}

	std::sort(&ret->pickup_actions[0], &ret->pickup_actions[ret->num_pickups], [city](const int p1, const int p2)
			{return (city->get_action(p1).exit_state & TRUCK_SIZE_MASK) < (city->get_action(p2).exit_state & TRUCK_SIZE_MASK); });
	std::sort(&ret->deliver_actions[0], &ret->deliver_actions[ret->num_delivers], [city](const int d1, const int d2)
			{return (city->get_action(d1).entr_state & TRUCK_SIZE_MASK) < (city->get_action(d2).entr_state & TRUCK_SIZE_MASK); });


	for (int i = 0; i < ret->num_delivers; i++)
		for (int j = 0; j < ret->num_pickups; j++)
			ret->d2p.at(i, j) = city->get_time_to(ret->deliver_actions[i], ret->pickup_actions[j]);

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

	for (int i = 0; i < city->num_stagingareas; i++)
		for (int j = 0; j < city->num_stagingareas; j++)
			ret->depot_2_depot.at(i, j) = city->durations.at(city->yards.at(i).location, city->yards.at(j).location);

#if 0
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
#endif

	return ret;
}
