/*
 * action.h
 *
 *  Created on: Oct 23, 2014
 *      Author: thallock
 */

#ifndef ACTION_H_
#define ACTION_H_

#include "common.h"

#include "model/landfill.h"
#include "model/yard.h"
#include "model/dumpstersize.h"
#include "model/operation.h"

class Action
{
public:
	Action(int loc);
	Action(const Landfill& l, dumpster_size s);
	Action(const Yard& l, dumpster_size in, dumpster_size out, operation o);
	~Action() {}

	operation op;
	dumpster_size in;
	dumpster_size out;
	int location;
	long begin_time;
	long   end_time;
	long wait_time;
	bool accessible[3];
	int value;

	friend std::ostream& operator<<(std::ostream& out, const Action& a);
};

#endif /* ACTION_H_ */
