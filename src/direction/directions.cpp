/*
 * directions.cpp
 *
 *  Created on: Nov 2, 2014
 *      Author: thallock
 */

#include "direction/directions.h"

#include <fstream>

directions::directions(const std::string& file)
{
	std::ifstream input { file };

	input >> departure_time;
	input >> duration;
	input >> distance;

	std::getline(input, address1);
	std::getline(input, address1);
	input >> begin;
	std::getline(input, address2);
	std::getline(input, address2);
	input >> end;

	do
	{
		int index = -1;
		input >> index;
		if (index != (int) steps.size())
		{
			break;
		}
		steps.push_back(step{input});
	} while (true);
}
