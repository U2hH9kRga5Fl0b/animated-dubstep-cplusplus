/*
 * depth_first_search.h
 *
 *  Created on: Oct 29, 2014
 *      Author: thallock
 */

#ifndef DEPTH_FIRST_SEARCH_H_
#define DEPTH_FIRST_SEARCH_H_

#include "model/solution.h"

#include <vector>
#include <functional>


struct ends
{
	bool operator()(const City* city, int action)
	{
		return get_truck_out_state(city, action) == TRUCK_STATE_NONE;
	}
};

struct begins
{
	bool operator()(const City* city, int action)
	{
		return get_truck_in_state(city, action) == TRUCK_STATE_NONE;
	}
};

struct has_value
{
	bool operator()(const City* city, int action)
	{
		return city->get_action(action).value;
	}
};

struct arrives_at
{
	arrives_at(int action) : lastaction{action} {}

	bool operator()(const City* city, int action)
	{
		return action == lastaction;
	}
private:
	int lastaction;
};


class insertion
{
public:
	int driver, start;
	std::vector<int> subpath;

	insertion(int driver_, int start_) : driver{driver_}, start{start_} {}
	~insertion() {}

	void apply(Solution* solution)
	{
		int num_stops = solution->get_number_of_stops(driver);
		if (start < num_stops)
		{
			err() << *solution << std::endl;
			err() << "not implemented!" << std::endl;
			trap();
		}
		int size = subpath.size();
		for (int i = 0; i < size; i++)
		{
			solution->append(driver, start + i, subpath.at(i));
		}
	}
};

bool search_for_path(Solution* solution, int end_index, int maxdepth, insertion& ins, std::function<bool(const City*, int action)> cri=has_value{});

#endif /* DEPTH_FIRST_SEARCH_H_ */
