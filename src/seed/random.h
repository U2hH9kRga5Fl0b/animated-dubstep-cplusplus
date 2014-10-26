/*
 * random.h
 *
 *  Created on: Oct 24, 2014
 *      Author: thallock
 */

#ifndef RANDOM_H_
#define RANDOM_H_





#include "model/solution.h"

Solution* get_random_solution(const City* c);
Solution* get_nearest_seed(const City* c);

bool apply_nearest_valuable(Solution*sol, int driver, int stop);

int get_best_path(const Solution* solution,
		int a1,
		int *out, int outlen);

#endif /* RANDOM_H_ */
