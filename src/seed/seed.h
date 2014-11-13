/*
 * random.h
 *
 *  Created on: Oct 24, 2014
 *      Author: thallock
 */

#ifndef RANDOM_H_
#define RANDOM_H_

#include "model/solution.h"

#if 0
Solution* get_random_solution_dfs(const City* c);
Solution* get_nearest_seed_dfs(const City* c);
#endif

Solution* get_random_solution_find(const City* c);
Solution* get_nearest_seed_find(const City* c);

void fix_endings(Solution* solution);


#endif /* RANDOM_H_ */
