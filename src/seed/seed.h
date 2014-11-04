/*
 * random.h
 *
 *  Created on: Oct 24, 2014
 *      Author: thallock
 */

#ifndef RANDOM_H_
#define RANDOM_H_

#include "model/solution.h"

Solution* get_random_solution_dfs(const City* c);
Solution* get_nearest_seed_dfs(const City* c);

Solution* get_random_solution_find(const City* c);
Solution* get_nearest_seed_find(const City* c);



#endif /* RANDOM_H_ */
