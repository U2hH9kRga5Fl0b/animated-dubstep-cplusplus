/*
 * nearest_find.cpp
 *
 *  Created on: Nov 9, 2014
 *      Author: thallock
 */

#include "seed/seed.h"

#include "opts/find_path.h"

#include <set>

#define NUM_NEAREST_TO_CONSIDER 5

namespace
{

class next_action_comparator
{
public:
	const City* city;
	int driver;
	int prev;
	next_action_comparator(const City* city_, int driver_, int prev_) :
		city{city_},
		driver{driver_},
		prev{prev_} {}

	bool operator()(int n1, int n2)
	{
		return city->get_time_to(driver, prev, n1) < city->get_time_to(driver, prev, n2);
	}
};



}

Solution* get_nearest_seed_find(const City* c)
{
	Solution* sol = new Solution { c, c->num_actions };

	std::vector<int> possibles;

	for (int i = 0; i < c->num_requests; i++)
		possibles.push_back(c->first_request_index + i);

	for (int i = 0; i < c->num_requests; i++)
	{
		int d = -1300;
		int min_time = INT_MAX;
		for (int j = 0; j < c->num_trucks; j++)
		{
			if (sol->get_time_for_driver(j) >= min_time)
				continue;
			min_time = sol->get_time_for_driver(j);
			d = j;
		}

		int length = sol->get_number_of_stops(d);
		int pa = sol->get_action_index(d, length-1);

		std::set<int, next_action_comparator> next_actions{next_action_comparator{c, d, pa}};
		auto end = possibles.end();
		for (auto it = possibles.begin(); it != end; ++it)
		{
			next_actions.insert(*it);
			if (next_actions.size() > NUM_NEAREST_TO_CONSIDER)
				next_actions.erase(--next_actions.end());
		}

		auto it = possibles.begin();
		std::advance(it, rand() % next_actions.size());
		int na = *it;

		int time;
		int len;
		int path[MAX_PATH];
		find_path_between_requests(sol, d, pa, na, &path[0], len, time);

		for (int j = 0; j < len; j++)
		{
			sol->append(d, length + j, path[j]);
		}
		sol->append(d, length + len, na);
	}

	fix_endings(sol);

	return sol;
}
