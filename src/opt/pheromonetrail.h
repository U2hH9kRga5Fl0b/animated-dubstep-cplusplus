/*
 * pheromonetrail.h
 *
 *  Created on: Oct 30, 2014
 *      Author: thallock
 */

#ifndef PHEROMONETRAIL_H_
#define PHEROMONETRAIL_H_

#include "model/solution.h"

class pheromone_trail
{
public:
	pheromone_trail(City* city, double evaporation_rate);
	~pheromone_trail();

	int get_next_action(int stop, bool* already_serviced);
	void update_pheromones(const Solution* solution);
private:
	const City* city;
	double **pheromones;
	double evaporation_rate;
	// scratch work
	int *tmp_indices;
	double *tmp_probs;
};

#endif /* PHEROMONETRAIL_H_ */
