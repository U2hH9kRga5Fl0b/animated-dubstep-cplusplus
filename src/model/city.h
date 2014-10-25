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

#include <vector>

class coord
{
public:
	coord();
	coord(double x, double y);
	coord(const coord& other);
	~coord();
	double dist(const coord& other) const;
	double x, y;
};

class city
{
public:
	city(int num_requests, int num_landfills, int num_stagingareas, int num_trucks);
	~city();

	inline int get_time_to(int from, int to) const
	{
		INBOUNDS(0, from, num_actions);
		INBOUNDS(0, to, num_actions);
		return durations.at(actions[from].location, actions[to].location) + actions[to].wait_time;
	}

	inline bool is_start_action(const int ndx) const
	{
		INBOUNDS(0, ndx, num_actions);
		return actions[ndx].op == Unstore || actions[ndx].op == Pickup;
	}

	int num_actions;
	int num_requests;
	int num_landfills;
	int num_stagingareas;
	int num_locations;
	int num_trucks;

	int start_location;

	const Action *actions;
	intarray durations;
	intarray possibles;
	coord *coords;
	truck_types* trucks;

	friend std::ostream& operator<<(std::ostream& out, const city& a);
private:
	std::vector<Action> donttouch;
};


#endif /* CITY_H_ */
