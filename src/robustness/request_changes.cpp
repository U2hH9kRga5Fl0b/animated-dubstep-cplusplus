/*
 * request_changes.cpp
 *
 *  Created on: Nov 8, 2014
 *      Author: thallock
 */


#include "robustness/request_changes.h"


#include "opts/requests_exchange.h"
#include "seed/seed.h"

#include "main/global.h"

City* reduce_city(const Solution* solution, int time)
{
	const City* old = solution->get_city();
	bool* request_filter = new bool[old->num_requests];

	for (int i = 0; i < old->num_requests; i++)
		request_filter[i] = false;

	int count = solution->mark_serviced(request_filter, time);

	Coord* coords = new Coord[solution->get_num_drivers()];
	int* actions = new int[solution->get_num_drivers()];

	take_still_shot(solution, time, coords, actions);

	for (int i = 0; i < old->num_trucks; i++)
	{
		actions[i] = old->get_action(actions[i], i).exit_state;
	}

	City* newcity = new City(old, request_filter, count, coords, actions);

	log() << "solutions already serviced: " << count << std::endl;
	log() << "old num requests: " << old->num_requests << std::endl;
	log() << "new num requests: " << newcity->num_requests << std::endl;

	viewer.show("original", old);
	viewer.show("reduced", newcity);
	viewer.pause();

	delete[] request_filter;
	delete[] coords;
	delete[] actions;

	return newcity;
}

void mutate_city(City* city, int num_location_changes, int num_operation_changes)
{
	for (int i = 0; i < num_location_changes; i++)
	{
		int action = city->first_request_index + rand() % city->num_requests;
		int location = city->get_action(action).location;
		city->coords[location].uniform(city->xmin, city->xmax, city->ymin, city->ymax);
	}

	for (int i = 0; i < num_operation_changes; i++)
	{
		int action = city->first_request_index + rand() % city->num_requests;
		if (state_is_full(city->get_action(action).exit_state))
		{
			// P ---> D
			dumpster_size size = get_state_size(city->get_action(action).exit_state);
			city->modify_action(action)->entr_state = get_state(TRUCK_STATE_EMPTY, size);
			city->modify_action(action)->exit_state = get_state(TRUCK_STATE_NONE,  none);
		}
		else
		{
			// D ----> P
			dumpster_size size = get_state_size(city->get_action(action).entr_state);
			city->modify_action(action)->entr_state = get_state(TRUCK_STATE_NONE, none);
			city->modify_action(action)->exit_state = get_state(TRUCK_STATE_FULL, size);
		}
	}
}

Solution* get_best_solution(const City* city)
{
	objective obj{1};
	Solution* sol = get_random_solution_find(city);
	while (apply_first_exchange(sol, &obj, true))
		;
	return sol;
}

void set_results(reduced_results* results, const City* city, int i)
{
	Solution* best = get_best_solution(city);
	results->total_times[i] = best->get_maximum_time();
	results->times[i] = rand() % results->total_times[i];
	City* reduced = reduce_city(best, results->times[i]);
	Solution *reduced_best = get_best_solution(reduced);
	results->reduced_times[i] = reduced_best->get_maximum_time();
	delete best; delete reduced; delete reduced_best;
}

reduced_results *get_results(City* city, int ntrails, int num_locations, int change_types)
{
	reduced_results *results = new reduced_results { ntrails };

	for (int i = 0; i < ntrails; i++)
	{
		// could be ran in parallel...
		set_results(results, city, i);
	}

	return results;
}
