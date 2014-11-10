/*
 * request_changes.h
 *
 *  Created on: Nov 8, 2014
 *      Author: thallock
 */

#ifndef REQUEST_CHANGES_H_
#define REQUEST_CHANGES_H_

#include "model/solution.h"

struct reduced_results
{
	int num_trials;

	int *total_times;
	int *times;
	int *reduced_times;

	reduced_results(int ntrials) :
		num_trials{ntrials},
		total_times{new int[ntrials]},
		times{new int[ntrials]},
		reduced_times{new int[ntrials]} {}

	~reduced_results()
	{
		delete[] total_times;
		delete[] times;
		delete[] reduced_times;
	}

	friend std::ostream& operator<<(std::ostream& out, const reduced_results& results)
	{
		out << "total:\n";
		for (int i = 0; i < results.num_trials; i++)
		{
			out << std::setw(5) << results.total_times[i] << ' ';
		}
		out << '\n';
		out << "cut time:\n";
		for (int i = 0; i < results.num_trials; i++)
		{
			out << std::setw(5) << results.times[i] << ' ';
		}
		out << '\n';
		out << "reduced:\n";
		for (int i = 0; i < results.num_trials; i++)
		{
			out << std::setw(5) << results.reduced_times[i] << ' ';
		}
		out << '\n';
		out << "diff:\n";
		for (int i = 0; i < results.num_trials; i++)
		{
			out << std::setw(5) << results.reduced_times[i] - (results.total_times[i] - results.times[i]) << ' ';
		}
		out << '\n';
		return out;
	}
};

City* reduce_city(const Solution* solution, int time);
void mutate_city(City* city, int num_location_changes, int num_operation_changes);

reduced_results *get_results(City* city, int ntrails, int num_locations, int change_types);


#endif /* REQUEST_CHANGES_H_ */
