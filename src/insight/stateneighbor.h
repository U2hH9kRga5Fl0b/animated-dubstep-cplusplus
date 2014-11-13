/*
 * stateneighbor.h
 *
 *  Created on: Nov 12, 2014
 *      Author: thallock
 */

#ifndef STATENEIGHBOR_H_
#define STATENEIGHBOR_H_

#include "insight/insight_state.h"


void apply_d2p_exchange(insight_state* state,
		int deliver1, int deliver2);

void apply_p2b_exchange(insight_state* state,
		int pickup_index, int driver);

void apply_d2e_exchange(insight_state* state,
		int deliver_index, int driver);

void apply_h2d_exchange(insight_state* state,
		int deliver_index, int hub);

void apply_p2h_exchange(insight_state* state,
		int pickup_index, int hub);



#endif /* STATENEIGHBOR_H_ */
