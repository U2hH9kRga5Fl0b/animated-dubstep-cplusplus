/*
 * insight_state.h
 *
 *  Created on: Nov 5, 2014
 *      Author: thallock
 */

#ifndef INSIGHT_STATE_H_
#define INSIGHT_STATE_H_

#include "insight/pairing_info.h"
#include "insight/interhub.h"

#include <list>

class insight_state
{
public:
	const pairing_info* info;

	int *deliver_depots;
	int *pickup_depots;
	int *delivers_to_pickups;

	std::list<interhub> inters;

	void fast_match();


	insight_state(const pairing_info* info);
	~insight_state();
};



#endif /* INSIGHT_STATE_H_ */
