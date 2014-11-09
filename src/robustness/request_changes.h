/*
 * request_changes.h
 *
 *  Created on: Nov 8, 2014
 *      Author: thallock
 */

#ifndef REQUEST_CHANGES_H_
#define REQUEST_CHANGES_H_


#include "model/solution.h"

City* reduce_city(const Solution* solution, int time);


void mutate_city(City* city);


#endif /* REQUEST_CHANGES_H_ */
