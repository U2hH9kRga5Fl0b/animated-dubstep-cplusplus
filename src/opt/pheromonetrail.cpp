/*
 * pheromonetrail.cpp
 *
 *  Created on: Oct 30, 2014
 *      Author: thallock
 */

#include "opt/pheromonetrail.h"

#include <cstdlib>


namespace
{
	constexpr double BIG_M = (TIME_IN_A_DAY * 10000.0);
}


pheromone_trail::pheromone_trail(City* city_, double evaporation_rate_) :
	city{city_},
	pheromones{new double*[city->num_actions]},
	evaporation_rate{evaporation_rate_},
	tmp_indices{new int[city->num_actions]},
	tmp_probs{new double[city->num_actions]}
{

	for (int i = 0; i < city->num_actions; i++)
	{
		int count = 0;
		for (int j = 0; j < city->num_actions; j++)
		{
			if (city->possibles.at(i, j) < 0)
			{
				count = j;
				break;
			}
		}
		pheromones[i] = new double[count];
		for (int j = 0; j < count; j++)
		{
			// prefer new solutions in the beginning
			pheromones[i][j] = BIG_M;
		}
	}
}

pheromone_trail::~pheromone_trail()
{
	for (int i = 0; i < city->num_actions; i++)
	{
		delete[] pheromones[i];
	}
	delete[] pheromones;
	delete[] tmp_indices;
	delete[] tmp_probs;
}

int pheromone_trail::get_next_action(int stop, bool* already_serviced)
{
	int len = 0;
	for (int i = 0; i < city->num_actions; i++)
	{
		int poss = city->possibles.at(stop, i);
		if (poss < 0)
		{
			break;
		}
		if (already_serviced[poss])
		{
			continue;
		}
		tmp_probs[len] = pheromones[stop][i] + (len == 0 ? 0 : tmp_probs[len - 1]);
		tmp_indices[len] = poss;
		len++;
	}

	if (len == 0)
	{
		err() << "Ran out of options!!!" << std::endl;
		trap();

	}

	double max = tmp_probs[len-1];
	double selection = max * (rand() / (double) RAND_MAX);
	for (int i = 0; i < len; i++)
	{
		if (tmp_probs[i] >= selection)
		{
			return i;
		}
	}

	err() << "couldn't select based on probs" << std::endl;
	trap();
	return len;
}

void pheromone_trail::update_pheromones(const Solution* solution)
{
//	double newvalue = solution->sum_all_times();
	for (int d = 0; d < solution->get_city()->num_trucks; d++)
	{
		int length = solution->get_number_of_stops(d);
		for (int s = 0; s < length; s++)
		{

		}
	}
}
