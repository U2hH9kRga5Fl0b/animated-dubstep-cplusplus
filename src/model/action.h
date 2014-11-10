/*
 * action.h
 *
 *  Created on: Oct 23, 2014
 *      Author: thallock
 */

#ifndef ACTION_H_
#define ACTION_H_

#include "common.h"

#include "model/truckstate.h"

class Yard;
class Landfill;

class Action
{
public:
	Action();
	Action(int loc, int idx);
	Action(const Landfill& l, dumpster_size s, int idx);
	Action(const Yard& y, truck_state in, truck_state out, int idx);
	~Action() {}

	int idx;

	truck_state entr_state;
	truck_state exit_state;

	int location;
	long begin_time;
	long end_time;
	long wait_time;
	bool accessible[3];
	int value;

	friend std::ostream& operator<<(std::ostream& out, const Action& a);
};

#endif /* ACTION_H_ */
