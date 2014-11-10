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

#define NUM_ACTIONS_PER_YARD 20
#define NUM_ACTIONS_PER_FILL  4

#define BEGIN_INDEX (-1)
#define END_INDEX   (-13)


inline int get_matching_staging_area_index(dumpster_size in, dumpster_size out);
inline int get_matching_landfill_index(dumpster_size s);


class City
{
public:
	City(int num_requests, int num_landfills, int num_stagingareas, int num_trucks);
	City(const City* other, bool *requests_completed, int count, Coord *locations, truck_state* states);

	~City();

#if 0
	inline int get_start_time(int first_action) const
	{
		INBOUNDS(0, first_action, num_actions);
		return durations.at(start_coord_index, actions[first_action].location) + actions[first_action].wait_time;
	}
#endif

	inline int get_time_to(int driver, int from, int to) const
	{
		int ret_val = 0;
		int from_loc;
		int to_loc;
		if (from == BEGIN_INDEX)
		{
			from_loc = begin_actions[driver].location;
		}
		else
		{
			INBOUNDS(0, from, num_actions);
			from_loc = actions[from].location;
		}
		if (to == END_INDEX)
		{
			to_loc = final_actions[driver].location;
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

	inline bool is_start_action(int driver, const int ndx) const
	{
		INBOUNDS(0, ndx, num_actions);
		return actions[ndx].entr_state == begin_actions[driver].exit_state;
	}

	inline const Action& get_action(int index, int driver=-1) const
	{
		if (index == BEGIN_INDEX)
		{
			INBOUNDS(0, driver, num_trucks);
			return begin_actions[driver];
		}
		if (index == END_INDEX)
		{
			INBOUNDS(0, driver, num_trucks);
			return final_actions[driver];
		}
		INBOUNDS(0, index, num_actions);
		return actions[index];
	}

	inline Action* modify_action(int index)
	{
		INBOUNDS(first_request_index, index, num_actions);
		return &donttouch.at(index - first_request_index);
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
				action == BEGIN_INDEX ||
				action == END_INDEX ||
				get_action(action, driverno).accessible[trucks[driverno]];
	}

	inline int get_staging_area_index(int staging_area, dumpster_size in, dumpster_size out) const
	{
		return first_stagingarea_index + NUM_ACTIONS_PER_YARD * staging_area + get_matching_staging_area_index(in, out);
	}
	inline int get_landfill_index(int fill, dumpster_size size) const
	{
		return first_landfill_index + NUM_ACTIONS_PER_FILL * fill + get_matching_landfill_index(size);
	}
	inline const Coord& get_start_location(int driver) const
	{
		INBOUNDS(0, driver, num_trucks);
		return coords[begin_actions[driver].location];
	}
	inline const Coord& get_end_location(int driver) const
	{
		INBOUNDS(0, driver, num_trucks);
		return coords[final_actions[driver].location];
	}

	std::string get_decription(int location) const;

	int num_actions;
	int num_requests;
	int num_landfills;
	int num_stagingareas;
	int num_locations;
	int num_trucks;

	intarray durations;
	intarray possibles;
	Coord *coords;
	truck_types* trucks;

	std::vector<Yard> yards;

	int first_landfill_index;
	int first_stagingarea_index;
	int first_request_index;

	Action* begin_actions;
	Action* final_actions;

	friend std::ostream& operator<<(std::ostream& out, const City& a);

	double xmax, ymax, xmin, ymin;

private:
	const Action *actions;
	std::vector<Action> donttouch;
	void common_init();
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
