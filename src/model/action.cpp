/*
 * action.cpp
 *
 *  Created on: Oct 23, 2014
 *      Author: thallock
 */

#include "model/action.h"

#include "model/landfill.h"
#include "model/yard.h"

#include <cstdlib>

namespace
{
	dumpster_size get_random_size()
	{
		dumpster_size sizes[] = { six, nine, nine, twelve, sixteen, nine, nine, nine, nine, nine, nine, nine, nine, };
		return sizes[rand() % 4];
	}
}

Action::Action(int loc) :
	entr_state{0},
	exit_state{0},
	location{loc},
#if ENFORCE_TIME_WINDOWS
	begin_time{rand() % 2 ? TIME_IN_A_DAY / 2 : 0},
	end_time {begin_time + TIME_IN_A_DAY / 2 },
#else
	begin_time{0},
	end_time {TIME_IN_A_DAY},
#endif
	wait_time{10 * 60},
	value{1}
{
	if (!(rand() % 10))
	{
		// replace
		entr_state |= TRUCK_STATE_EMPTY;
		entr_state |= TRUCK_SIZE_MASK & get_random_size();
		exit_state |= TRUCK_STATE_FULL;
		exit_state |= TRUCK_SIZE_MASK & get_random_size();
	}
	else if (rand() % 2)
	{
		// Pickup
		entr_state |= TRUCK_STATE_NONE;
		exit_state |= TRUCK_STATE_FULL;
		exit_state |= TRUCK_SIZE_MASK & get_random_size();
	}
	else
	{
		// Dropoff
		entr_state |= TRUCK_STATE_EMPTY;
		entr_state |= TRUCK_SIZE_MASK & get_random_size();
		exit_state |= TRUCK_STATE_NONE;
	}
	accessible[0] = accessible[1] = accessible[2] = true;
	if (get_state_size(exit_state) == sixteen && state_is_full(exit_state))
	{
		accessible[0] = false;
	}
}

Action::Action(const Landfill& l, dumpster_size s) :
	entr_state{TRUCK_STATE_FULL  | s},
	exit_state{TRUCK_STATE_EMPTY | s},
	location{l.location},
	begin_time{0},
	end_time {TIME_IN_A_DAY},
	wait_time{l.waittime},
	value{0}
{
	accessible[0] = accessible[1] = accessible[2] = true;
}

Action::Action(const Yard& y, truck_state in, truck_state out) :
	entr_state{in},
	exit_state{out},
	location{y.location},
	begin_time{0},
	end_time {TIME_IN_A_DAY},
	wait_time{15 * 60},
	value{0}
{
	accessible[0] = accessible[1] = accessible[2] = true;
}

#define ac(x) (a.accessible[x]?'1':'0')

std::ostream& operator<<(std::ostream& out, const Action& a)
{
	return out
	    << '[' << get_truck_state_desc(a.entr_state) << ']'
	    << '[' << get_truck_state_desc(a.exit_state) << ']'
	    << '[' << std::setw(3) << a.location << ']'
	    << '[' << std::setw(5) << a.begin_time << '-' << std::setw(5) << a.end_time << ']'
	    << '[' << std::setw(4) << a.wait_time << ']'
	    << '[' << a.value << ']'
	    << '[' << ac(0) << ac(1) << ac(2) << ']';
}
