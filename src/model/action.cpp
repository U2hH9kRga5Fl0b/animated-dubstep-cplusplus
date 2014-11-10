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

Action::Action(int loc, int idx_) :
	idx{idx_},
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
	wait_time{TIME_FOR_REQUEST},
	value{1}
{
	// temporarily disable replaces...
	if (0 && !(rand() % 10))
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

Action::Action() :
		idx{-1},
		entr_state{TRUCK_STATE_NONE},
		exit_state{TRUCK_STATE_NONE},
		location{-1},
		begin_time{0},
		end_time {TIME_IN_A_DAY},
		wait_time{0},
		value{0}
{
	for (int i = 0; i < 3; i++)
		accessible[i] = true;
}


Action::Action(const Landfill& l, dumpster_size s, int idx_) :
	idx{idx_},
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

Action::Action(const Yard& y, truck_state in, truck_state out, int idx_) :
	idx{idx_},
	entr_state{in},
	exit_state{out},
	location{y.location},
	begin_time{0},
	end_time {TIME_IN_A_DAY},
	wait_time{TIME_FOR_STAGING_AREA},
	value{0}
{
	accessible[0] = accessible[1] = accessible[2] = true;
}

#define ac(x) (a.accessible[x]?'1':'0')

std::ostream& operator<<(std::ostream& out, const Action& a)
{
	return out
	    << "[" << "enter:" << get_truck_state_desc(a.entr_state)                                 << ']'
	    << "[" << "exit:" << get_truck_state_desc(a.exit_state)                                  << ']'
	    << "[" << "loc: " << std::setw(3) << a.location                                          << ']'
	    << "[" << "window:" << std::setw(5) << a.begin_time << '-' << std::setw(5) << a.end_time << ']'
	    << "[" << "time:" << std::setw(4) << a.wait_time                                         << ']'
	    << "[" << "is req:" << a.value                                                           << ']'
	    << "[" << "ndx:" << std::setw(3) << a.idx                                                << ']'
	    << "[" << "trucks:" << ac(0) << ac(1) << ac(2)                                           << ']';
}
