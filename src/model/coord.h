/*
 * coord.h
 *
 *  Created on: Oct 31, 2014
 *      Author: thallock
 */

#ifndef COORD_H_
#define COORD_H_

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

#endif /* COORD_H_ */
