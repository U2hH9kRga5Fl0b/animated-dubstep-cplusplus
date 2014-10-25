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

class action
{
public:
	action(int loc);
	action(const landfill& l, dumpster_size s);
	action(const yard& l, dumpster_size in, dumpster_size out, operation o);
	~action() {}

	operation op;
	dumpster_size in;
	dumpster_size out;
	int location;
	long begin_time;
	long   end_time;
	long wait_time;
	bool accessible[3];
	int value;

	friend std::ostream& operator<<(std::ostream& out, const action& a);
};

#endif /* ACTION_H_ */
