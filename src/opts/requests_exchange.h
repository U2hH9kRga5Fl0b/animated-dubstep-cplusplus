/*
 * requests_stop.h
 *
 *  Created on: Nov 3, 2014
 *      Author: thallock
 */

#ifndef REQUESTS_STOP_H_
#define REQUESTS_STOP_H_

#include "opts/neighbor.h"

bool apply_random_request_exchange(Solution* solution, int num_attempts, const objective* obj);
bool apply_first_exchange(Solution* solution, const objective* obj, bool cont=true);

void request_exchange_search(Solution* solution, const objective* obj);

#endif /* REQUESTS_STOP_H_ */
