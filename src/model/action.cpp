/*
 * action.cpp
 *
 *  Created on: Oct 23, 2014
 *      Author: thallock
 */

#include "model/action.h"

#include <cstdlib>

#define TOTAL_TIME (60 * 60 * 12)

namespace
{
	operation get_random_operation()
	{
		if (rand() % 3)
		{
			return Replace;
		}
		else if (rand() % 2)
		{
			return Pickup;
		}
		else
		{
			return Dropoff;
		}
	}

	dumpster_size get_random_size()
	{
		dumpster_size sizes[] = { six, nine, twelve, sixteen, };
		return sizes[rand() % 4];
	}
}

action::action(int loc) :
	op{get_random_operation()},
	in{op == Pickup ? none : get_random_size()},
	out{op == Dropoff ? none : get_random_size()},
	location{loc},
	begin_time{rand() % 2 ? TOTAL_TIME / 2 : 0},
	end_time {begin_time + TOTAL_TIME / 2 },
	wait_time{10 * 60},
	value{1}
{
	accessible[0] = accessible[1] = accessible[2] = true;
	if (out == sixteen && (op == Pickup || op == Replace))
	{
		accessible[0] = false;
	}
}

action::action(const landfill& l, dumpster_size s) :
	op{Dump},
	in{s},
	out{s},
	location{l.location},
	begin_time{0},
	end_time {TOTAL_TIME},
	wait_time{l.waittime},
	value{0}
{
	accessible[0] = accessible[1] = accessible[2] = true;
}

action::action(const yard& y, dumpster_size in, dumpster_size out, operation o) :
	op{o},
	in{in},
	out{out},
	location{y.location},
	begin_time{0},
	end_time {TOTAL_TIME},
	wait_time{15 * 60},
	value{0}
{
	accessible[0] = accessible[1] = accessible[2] = true;
}

#define ac(x) (a.accessible[x]?'1':'0')

std::ostream& operator<<(std::ostream& out, const action& a)
{
	return out
	    << '[' << operation_to_char(a.op) << ']'
	    << '[' << std::setw(2) << a.in << "->" << std::setw(2) << a.out << ']'
	    << '[' << std::setw(3) << a.location << ']'
	    << '[' << std::setw(5) << a.begin_time << '-' << std::setw(5) << a.end_time << ']'
	    << '[' << std::setw(4) << a.wait_time << ']'
	    << '[' << ac(0) << ac(1) << ac(2) << ']';
}
