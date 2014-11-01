/*
 * ensure_feasible.cpp
 *
 *  Created on: Oct 29, 2014
 *      Author: thallock
 */

#include "main/global.h"

void ensure_feasible(Solution* solution)
{
	int num_drivers = solution->get_num_drivers();
	for (int d=0; d<num_drivers; d++)
	{
		int length = solution->get_number_of_stops(d);
		for (int s = 0; s < length; s++)
		{

		}
	}
}
