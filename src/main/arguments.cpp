/*
 * arguments.cpp
 *
 *  Created on: Oct 29, 2014
 *      Author: thallock
 */

#include "main/main.h"

#include <fstream>

City* city;
std::string infile;
std::string outfile;

void parse_args(int argc, char **argv)
{
	bool found_city = false;

	for (int i = 1; i < argc; i++)
	{
		std::string str{argv[i]};

		if (str == "--infile")
		{
			// better have an i++ arg...
			infile = std::string{argv[i++]};
		}
		if (str == "--outfile")
		{
			outfile = std::string{argv[i++]};
		}
		if (str == "--city")
		{
			// ...
			// found_city = true;
		}
		if (str == "--seed")
		{
			srand(atoi(argv[i++]));
		}
	}

	if (!found_city)
	{
		city = new City{5, 1, 1, 2};
		{
			std::ofstream os{"city.txt"};
			os << *city << std::endl;
			log() << *city << std::endl;
		}
	}
}
