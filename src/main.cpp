/*
 * main.cpp
 *
 *  Created on: Oct 24, 2014
 *      Author: thallock
 */

#include "model/city.h"
#include "seed/random.h"

#include <fstream>

int main(int argc, char **argv)
{
	city c{70,4,4,10};
	{
		std::ofstream os{"city.txt"};
		os << c << std::endl;
		std::cout << c << std::endl;
	}

	solution* sol = get_random_solution(&c);

	std::cout << *sol << std::endl;

	sol->human_readable(std::cout);
}

