/*
 * coord.h
 *
 *  Created on: Oct 31, 2014
 *      Author: thallock
 */

#ifndef COORD_H_
#define COORD_H_

#include <iostream>
#include <iomanip>

class Coord
{
public:
	Coord();
	Coord(double x, double y);
	Coord(const Coord& other);
	~Coord();

	void uniform(double xmin, double xmax, double ymin, double ymax);

	double dist(const Coord& other) const;
	double x, y;

	friend std::ostream& operator<<(std::ostream& out, const Coord& c)
	{
		return out << std::setw(10) << c.x << ", " << std::setw(10) << c.y;
	}
};

#endif /* COORD_H_ */
