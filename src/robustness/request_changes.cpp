/*
 * request_changes.cpp
 *
 *  Created on: Nov 8, 2014
 *      Author: thallock
 */


#include "robustness/request_changes.h"

City* reduce_city(const Solution* solution, int time)
{
	const City* old = solution->get_city();
	bool* request_filter = new bool[old->num_requests];

	for (int i = 0; i < old->num_requests; i++)
	{
		request_filter[i] = false;
	}

	solution->mark_serviced(request_filter, time);

	City* newcity = new City(old, request_filter);

	delete[] request_filter;

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
