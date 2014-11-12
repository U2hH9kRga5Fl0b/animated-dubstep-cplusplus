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

	// determines inventories

	int *deliver_depots;
	int *pickup_depots;

	int *delivers_to_pickups;

	int *unmatched_delivers;
	int *unmatched_pickups;

	interhub *embark;
	interhub *finish;

	insight_state(const pairing_info* info);
	insight_state(const pairing_info* info, const Solution* solution);
	~insight_state();

	void fast_match();
	friend std::ostream& operator<<(std::ostream& out, const insight_state& state);

};



#endif /* INSIGHT_STATE_H_ */
