/*
 * directions.h
 *
 *  Created on: Nov 2, 2014
 *      Author: thallock
 */

#ifndef DIRECTIONS_H_
#define DIRECTIONS_H_

#include "direction/step.h"

#define YEAR_2000  946688461
#define YEAR_2020 1577840461

class directions
{
public:
	directions(const std::string& file);

	double get_cost(location start, location stop)
	{
		return std::max(begin.euclidean(start), end.euclidean(stop));
	}

	friend std::ostream& operator<<(std::ostream& out, const directions& db)
	{
		return out << "from [" << db.address1 << "] to [" << db.address2 << "]";
	}

	bool valid()
	{
		return address1.size() != 0 && departure_time > YEAR_2000 && departure_time < YEAR_2020;
	}

	int departure_time;

	std::string address1;
	std::string address2;

	location begin;
	location end;

	int duration;
	int distance;

	std::list<step> steps;
};

#endif /* DIRECTIONS_H_ */
