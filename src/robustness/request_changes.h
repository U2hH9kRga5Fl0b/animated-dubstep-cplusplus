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

	int *total_times1;
	int *total_times2;
	int *cut_times;
	int *max_times;
	int *reduced_times1;
	int *reduced_times2;

	reduced_results(int ntrials) :
		num_trials{ntrials},
		total_times1    {new int[ntrials]},
		total_times2    {new int[ntrials]},
		cut_times       {new int[ntrials]},
		max_times       {new int[ntrials]},
		reduced_times1  {new int[ntrials]},
		reduced_times2  {new int[ntrials]} {}

	~reduced_results()
	{
		delete[] total_times1;
		delete[] total_times2;
		delete[] cut_times;
		delete[] max_times;
		delete[] reduced_times1;
		delete[] reduced_times2;
	}

	friend std::ostream& operator<<(std::ostream& out, const reduced_results& results)
	{
		constexpr int WIDTH = 6;

		out << "total:\n";
		for (int i = 0; i < results.num_trials; i++)
		{
			out << "[" << std::setw(WIDTH) << results.total_times1[i] << ':' << std::setw(WIDTH) << results.total_times2[i] << "] ";
		}
		out << '\n';
		out << "cut time:\n";
		for (int i = 0; i < results.num_trials; i++)
		{
			out << "[" << std::setw(WIDTH) << results.cut_times[i] << ':' << std::setw(WIDTH) << results.max_times[i] << "] ";
		}
		out << '\n';
		out << "reduced:\n";
		for (int i = 0; i < results.num_trials; i++)
		{
			out << "[" << std::setw(WIDTH) << results.reduced_times1[i] << ':' << std::setw(WIDTH) << results.reduced_times2[i] << "] ";
		}
		out << '\n';
		out << "diff:\n";
		for (int i = 0; i < results.num_trials; i++)
		{
			if (results.total_times1[i] > results.total_times2[i])
			{
				out << '[' << std::setw(WIDTH) << (results.total_times1[i] - results.total_times2[i])
						<< ':' << std::setw(WIDTH) << (results.reduced_times1[i] - results.reduced_times2[i]) << "] ";
			}
			else
			{
				out << '[' << std::setw(WIDTH) << (results.total_times2[i] - results.total_times1[i])
						<< ':' << std::setw(WIDTH) << (results.reduced_times2[i] - results.reduced_times1[i]) << "] ";
			}
		}
		out << '\n';
		return out;
	}
};

City* reduce_city(const Solution* solution, int time);
void mutate_city(City* city, int num_location_changes, int num_operation_changes);

void get_results(const std::string& filename);


#endif /* REQUEST_CHANGES_H_ */
