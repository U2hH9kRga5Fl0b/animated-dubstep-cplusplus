/*
 * scatter.h
 *
 *  Created on: Oct 29, 2014
 *      Author: thallock
 */

#ifndef SCATTER_H_
#define SCATTER_H_


#include "model/solution.h"

#include <functional>


int get_distance(const Solution* solution, const Solution* solution2);
bool relink(const Solution* solution, const Solution* solution2, std::function<void(const Solution*s)> callback);


#endif /* SCATTER_H_ */
