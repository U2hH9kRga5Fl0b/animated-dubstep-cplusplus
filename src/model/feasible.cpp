/*
 * feasible.cpp
 *
 *  Created on: Oct 26, 2014
 *      Author: thallock
 */

#include "model/solution.h"


bool is_feasible(Solution* solution)
{
	int num_drivers = solution->get_num_drivers();
	for (int driver = 0; driver < num_drivers; driver++)
	{
		int length = solution->get_length(driver);


	}
}
