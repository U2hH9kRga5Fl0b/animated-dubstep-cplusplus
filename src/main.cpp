/*
 * main.cpp
 *
 *  Created on: Oct 24, 2014
 *      Author: thallock
 */

#include "model/city.h"
#include "seed/random.h"
#include "viz/vizualizer.h"

#include <fstream>

#include "common.h"

// add local search


int main(int argc, char **argv)
{
	srand(5000015);

	City c{1000,20,20,10};
	{
		std::ofstream os{"city.txt"};
		os << c << std::endl;
		std::cout << c << std::endl;
	}

	Solution* sol = get_nearest_seed(&c);
	std::cout << *sol << std::endl;
	sol->human_readable(std::cout);

	vizualizer v {"foobar"};
	v.show(sol);
	std::cout << "should be showing." << std::endl;
	v.pause(1000);

	int last_time = sol->get_last_time() + 10 * 60;

	for (int t = 0; t < last_time; t += 20)
	{
		v.show(sol, t);
	}
}

