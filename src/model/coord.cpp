/*
 * coord.cpp
 *
 *  Created on: Oct 31, 2014
 *      Author: thallock
 */

#include "model/coord.h"

#include <cstdlib>
#include <cmath>

Coord::Coord(double x_, double y_) : x{x_}, y{y_} {}
Coord::Coord(const Coord& other) : Coord{other.x, other.y} {}
Coord::Coord() : Coord{0, 0} {}

Coord::~Coord() {}

double Coord::dist(const Coord& other) const
{
	double dx = other.x - x;
	double dy = other.y - y;
	return std::sqrt(dx * dx + dy * dy);
}

void Coord::uniform(double xmin, double xmax, double ymin, double ymax)
{
	x = xmin + (rand() / (double) RAND_MAX) * (xmax - xmin);
	y = ymin + (rand() / (double) RAND_MAX) * (ymax - ymin);
}

