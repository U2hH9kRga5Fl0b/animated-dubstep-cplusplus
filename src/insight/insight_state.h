/*
 * insight_state.h
 *
 *  Created on: Nov 5, 2014
 *      Author: thallock
 */

#ifndef INSIGHT_STATE_H_
#define INSIGHT_STATE_H_

#include "model/solution.h"
#include "insight/pairing_info.h"
#include "insight/interhub.h"

#include <list>
#include <set>

extern std::set<int> AVOID_NO_ACTIONS;

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

	void match(int deliver_index, int pickup_index);

	void fast_match(const std::set<int>& avoid_actions=AVOID_NO_ACTIONS);
	friend std::ostream& operator<<(std::ostream& out, const insight_state& state);

	int get_cost() const;
	insight_state& operator=(const insight_state& other);


	// ways to mutate:
	// split D->P
	//
};



#endif /* INSIGHT_STATE_H_ */
