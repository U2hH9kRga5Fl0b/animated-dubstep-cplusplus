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

#include <thread>
#include <fstream>





#include "opts/requests_exchange.h"

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

//	log() << "solutions already serviced: " << count << std::endl;
//	log() << "old num requests: " << old->num_requests << std::endl;
//	log() << "new num requests: " << newcity->num_requests << std::endl;

//	viewer.show("original", old);
//	viewer.show("reduced", newcity);
//	viewer.pause();

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

void single_solve(const City* city, Solution** ret)
{
	std::string myname = c("tmp", rand());
	objective obj;
	Solution* sol = get_random_solution_find(city);
	while (apply_first_exchange(sol, &obj, true))
		;
	(*ret) = sol;
}

Solution* get_best_solution(const City* city, int number_of_tries)
{
#if 0
	Solution* best = nullptr;
	constexpr int NUM_THREADS = 9;

	std::thread ** threads = new std::thread*[NUM_THREADS];
	Solution** sols = new Solution*[NUM_THREADS];

	for (int g = 0; g < number_of_tries; g += NUM_THREADS)
	{
		int num_in_group = std::min(NUM_THREADS, number_of_tries - g);

		for (int i = 0; i < num_in_group; i++)
			threads[i] = new std::thread {single_solve, city, &sols[i]};

		for (int i = 0; i < num_in_group; i++)
			threads[i]->join();

		for (int i = 0; i < num_in_group; i++)
			delete threads[i];

		for (int i = 0; i < num_in_group; i++)
		{

			if (best == nullptr || sols[i]->sum_all_times() < best->sum_all_times())
			{
				delete best;
				best = sols[i];
			}
			else
			{
				delete sols[i];
			}
		}
	}
	return best;
#else
	Solution* best = nullptr;
	int best_time = INT_MAX;

	for (int i = 0; i < number_of_tries; i++)
	{
		objective obj;
		Solution* sol = get_random_solution_find(city);
		while (apply_first_exchange(sol, &obj, true))
			;


		int alternative_time = get_max_time(sol);

		if (best == nullptr || alternative_time < best_time)
		{
			delete best;
			best = sol;
		}
		else
		{
			delete sol;
		}
	}

	return best;
#endif
}

std::string save_results(int i, int num_to_run)
{
	std::stringstream results;
	results << "" << i << " ";

	City* city = new City { 50, 5, 5, 10 };
	viewer.snapshot("o", city, "robust/" + c(i, "_original"));

	log () << "finding best for city1" << std::endl;
	Solution* alternative1 = get_best_solution(city, 1);
	log () << "finding best for city2" << std::endl;
	Solution* alternative2 = get_best_solution(city, 1);
	viewer.snapshot("a1", alternative1, "robust/" + c(i, "_alternate_1"));
	viewer.snapshot("a2", alternative2, "robust/" + c(i, "_alternate_2"));

	int total_times1 = alternative1->sum_all_times();
	int total_times2 = alternative2->sum_all_times();

	int max_time = std::min(alternative1->get_maximum_time(), alternative2->get_maximum_time());

	results << total_times1 << " " << total_times2 << " " << max_time << " " << num_to_run << " ";

	for (int j = 0; j < num_to_run; j++)
	{
		int cut_time = rand() % max_time;

		City* reduced1 = reduce_city(alternative1, cut_time);
		City* reduced2 = reduce_city(alternative2, cut_time);
		viewer.snapshot("c1", reduced1, "robust/" + c(j, "_" + c(i, "_reduced_city_1")));
		viewer.snapshot("c2", reduced2, "robust/" + c(j, "_" + c(i, "_reduced_city_2")));

		int num_to_mutate = std::min(5, std::min(reduced1->num_requests, reduced2->num_requests) / 4);
		mutate_city(reduced1, num_to_mutate, 0);
		mutate_city(reduced2, num_to_mutate, 0);
		viewer.snapshot("m1", reduced1, "robust/" +c(j, "_" +  c(i, "_mutated_city_1")));
		viewer.snapshot("m2", reduced2, "robust/" +c(j, "_" +  c(i, "_mutated_city_2")));

		Solution *reduced_best1 = get_best_solution(reduced1, 160);
		Solution *reduced_best2 = get_best_solution(reduced2, 160);
		viewer.snapshot("r1", reduced_best1, "robust/" + c(j, "_" + c(i, "_mutated_solution_1")));
		viewer.snapshot("r2", reduced_best2, "robust/" + c(j, "_" + c(i, "_mutated_solution_2")));

		int reduced_time1 = reduced_best1->sum_all_times();
		int reduced_time2 = reduced_best2->sum_all_times();

		results << cut_time << " " << reduced_time1 << " " << reduced_time2 << " ";

		delete reduced1; delete reduced2; delete reduced_best1; delete reduced_best2;

		log() << "done with test " << j << " of " << num_to_run << " for trial " << i << std::endl;
	}

	delete city; delete alternative1; delete alternative2;

	results << '\n';

	return results.str();
}

void get_results(const std::string& filename)
{
	constexpr int ntrials = 1;
	std::ofstream res {filename};

	// could be ran in parallel...
	for (int i = 0; i < ntrials; i++)
	{
		res << save_results(i, 50);
		log() << "done with set " << i << " of " << ntrials << std::endl;
	}
}
