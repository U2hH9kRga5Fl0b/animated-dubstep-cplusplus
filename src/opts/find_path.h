/*
 * find_path.h
 *
 *  Created on: Nov 3, 2014
 *      Author: thallock
 */

#ifndef FIND_PATH_H_
#define FIND_PATH_H_

#include "model/solution.h"

#include <algorithm>

#define MAX_PATH 2

void find_path_between_requests(const Solution* solution, int a1, int a2, int *path, int& len, int& time);

#endif /* FIND_PATH_H_ */
