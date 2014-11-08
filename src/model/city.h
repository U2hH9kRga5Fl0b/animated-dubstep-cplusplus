/*
 * city.h
 *
 *  Created on: Oct 23, 2014
 *      Author: thallock
 */

#ifndef CITY_H_
#define CITY_H_

#include "model/action.h"
#include "model/trucktypes.h"
#include "intarray.h"
#include "model/yard.h"
#include "model/coord.h"

#include <vector>

#define START_ACTION_INDEX -97
#define END_ACTION_INDEX   -23
extern Action sentinal_action;

#define NUM_ACTIONS_PER_YARD 20
#define NUM_ACTIONS_PER_FILL  4

inline int get_matching_staging_area_index(dumpster_size in, dumpster_size out);
inline int get_matching_landfill_index(dumpster_size s);

class City
{
public:
	City(int num_requests, int num_landfills, int num_stagingareas, int num_trucks);
	~City();

	inline int get_start_time(int first_action) const
	{
		INBOUNDS(0, first_action, num_actions);
		return durations.at(start_coord_index, actions[first_action].location) + actions[first_action].wait_time;
	}

	inline int get_time_to(int from, int to) const
	{
		int ret_val = 0;
		int from_loc;
		int to_loc;
		if (from == START_ACTION_INDEX)
		{
			from_loc = start_coord_index;
		}
		else
		{
			INBOUNDS(0, from, num_actions);
			from_loc = actions[from].location;
		}
		if (to == END_ACTION_INDEX)
		{
			to_loc = start_coord_index;
		}
		else
		{
			INBOUNDS(0, to, num_actions);
			to_loc = actions[to].location;
			ret_val += actions[to].wait_time;
		}
		ret_val += durations.at(from_loc, to_loc);
		return ret_val;
	}

	inline bool is_start_action(const int ndx) const
	{
		INBOUNDS(0, ndx, num_actions);
		return actions[ndx].entr_state == TRUCK_STATE_NONE;
	}

	inline const Action& get_action(int index) const
	{
		if (index == START_ACTION_INDEX || index == END_ACTION_INDEX) return sentinal_action;
		INBOUNDS(0, index, num_actions);
		return actions[index];
	}

	inline bool is_landfill(int action) const
	{
		INBOUNDS(0, action, num_actions);
		return action >= first_landfill_index && action < first_stagingarea_index;
	}
	inline bool is_staging_area(int action) const
	{
		INBOUNDS(0, action, num_actions);
		return first_stagingarea_index >= 0 && action < first_request_index;
	}
	inline bool is_request(int action) const
	{
		INBOUNDS(0, action, num_actions);
		return action >= first_request_index && action < num_actions;
	}

	inline bool driver_can_service(int driverno, int action) const
	{
		return
				action == START_ACTION_INDEX ||
				action == END_ACTION_INDEX ||
				get_action(action).accessible[trucks[driverno]];
	}

	inline int get_staging_area_index(int staging_area, dumpster_size in, dumpster_size out) const
	{
		return first_stagingarea_index + NUM_ACTIONS_PER_YARD * staging_area + get_matching_staging_area_index(in, out);
	}
	inline int get_landfill_index(int fill, dumpster_size size) const
	{
		return first_landfill_index + NUM_ACTIONS_PER_FILL * fill + get_matching_landfill_index(size);
	}

	std::string get_decription(int location) const;

	int num_actions;
	int num_requests;
	int num_landfills;
	int num_stagingareas;
	int num_locations;
	int num_trucks;

	int start_coord_index;
	int start_staging_area;

	intarray durations;
	intarray possibles;
	Coord *coords;
	truck_types* trucks;

	std::vector<Yard> yards;

	int first_landfill_index;
	int first_stagingarea_index;
	int first_request_index;

	friend std::ostream& operator<<(std::ostream& out, const City& a);

	double xmax, ymax, xmin, ymin;
private:
	const Action *actions;
	std::vector<Action> donttouch;
};
























inline int get_matching_landfill_index(dumpster_size s)
{
	switch (s)
	{
		case none:     trap(); return -1;
		case six:      return 0;
		case nine:     return 1;
		case twelve:   return 2;
		case sixteen:  return 3;
		default:       trap(); return -1;
	}
}

inline int get_matching_staging_area_index(dumpster_size in, dumpster_size out)
{
	switch(in)
	{
		case none:
			switch(out)
			{
				case none:    trap(); break;
				case six:     return  4;
				case nine:    return  5;
				case twelve:  return  6;
				case sixteen: return  7;
				default:      trap(); break;
			} break;
		case six:
			switch(out)
			{
				case none:    return  0;
				case six:     trap(); break;
				case nine:    return  8;
				case twelve:  return  9;
				case sixteen: return 10;
				default:      trap(); break;
			} break;
		case nine:
			switch(out)
			{
				case none:    return  1;
				case six:     return 11;
				case nine:    trap(); break;
				case twelve:  return 12;
				case sixteen: return 13;
				default:      trap(); break;
			} break;
		case twelve:
			switch(out)
			{
				case none:    return  2;
				case six:     return 14;
				case nine:    return 15;
				case twelve:  trap(); break;
				case sixteen: return 16;
				default:      trap(); break;
			} break;
		case sixteen:
			switch(out)
			{
				case none:    return  3;
				case six:     return 17;
				case nine:    return 18;
				case twelve:  return 19;
				case sixteen: trap(); break;
				default:      trap(); break;
			} break;
		default: trap();
	}

	trap();
	return -1;
}



#endif /* CITY_H_ */
