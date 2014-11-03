/*
 * location.h
 *
 *  Created on: Nov 2, 2014
 *      Author: thallock
 */

#ifndef LOCATION_H_
#define LOCATION_H_

#include <iostream>
#include <cmath>

class location
{
public:
	location() : lng{-1}, lat{-1} {}
	location(double x, double y) : lng{y}, lat{x} {}

	double lng;
	double lat;

	double euclidean(const location& other)
	{
		double dx = lat - other.lat;
		double dy = lng - other.lng;

		return std::sqrt(dx*dx + dy*dy);
	}

	friend std::ostream& operator<<(std::ostream& out, const location& l)
	{
		return out << "[" << l.lat << ", " << l.lng << "]";
	}

	friend std::istream& operator>>(std::istream& in, location& l)
	{
		in >> l.lng;
		in >> l.lat;
		return in;
	}
};

#endif /* LOCATION_H_ */
