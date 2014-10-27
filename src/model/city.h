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

#include <vector>

class Coord
{
public:
	Coord();
	Coord(double x, double y);
	Coord(const Coord& other);
	~Coord();
	double dist(const Coord& other) const;
	double x, y;
};

class City
{
public:
	City(int num_requests, int num_landfills, int num_stagingareas, int num_trucks);
	~City();

	inline int get_start_time(int first_action) const
	{
		INBOUNDS(0, first_action, num_actions);
		return durations.at(start_location, actions[first_action].location) + actions[first_action].wait_time;
	}

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

	inline const Action& get_action(int index) const
	{
		INBOUNDS(0, index, num_actions);
		return actions[index];
	}

	inline bool is_staging_area(int action) const
	{
		INBOUNDS(0, action, num_actions);
		return actions[action].op == Unstore || actions[action].op == Store;
	}

	std::string get_decription(int location) const;



	int num_actions;
	int num_requests;
	int num_landfills;
	int num_stagingareas;
	int num_locations;
	int num_trucks;

	int start_location;

	intarray durations;
	intarray possibles;
	Coord *coords;
	truck_types* trucks;

	std::vector<Yard> yards;

	friend std::ostream& operator<<(std::ostream& out, const City& a);

private:
	const Action *actions;
	std::vector<Action> donttouch;
};


#endif /* CITY_H_ */
