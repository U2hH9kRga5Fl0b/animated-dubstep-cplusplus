/*
 * step.cpp
 *
 *  Created on: Nov 2, 2014
 *      Author: thallock
 */

#include "direction/step.h"

#include <sstream>

#define IS_PART_OF_NUMBER(x)                        \
	(                                           \
			(x > '9' || x < '0') &&     \
			(x != '.') &&               \
			(x != '-')                  \
	)

step::step(std::istream& input) :
		duration { -1 }, distance { -1 }
{
	std::string line;
	std::getline(input, line);
	{
		std::getline(input, line);
		std::stringstream ss { line };
		ss >> duration;
		ss >> distance;
	}

	{
		std::getline(input, line);
		std::stringstream ss { line };
		ss >> begin;
	}

	{
		std::getline(input, line);
		std::stringstream ss { line };
		ss >> end;
	}

	std::getline(input, line);
	std::stringstream ss { line };
	do
	{
		double lat, lng;
		if (!ss)
		{
			break;
		}
		ss >> lng;
		if (!ss)
		{
			break;
		}
		ss >> lat;

		path.push_back(location{lat, lng});
	}
	while (true);
}

