/*
 * requests_stop.h
 *
 *  Created on: Nov 3, 2014
 *      Author: thallock
 */

#ifndef REQUESTS_STOP_H_
#define REQUESTS_STOP_H_

#include "model/solution.h"


bool apply_random_request_exchange(Solution* solution, int num_attempts);
bool apply_first_exchange(Solution* solution);

void request_exchange_search(Solution* solution);

#endif /* REQUESTS_STOP_H_ */
