/*
 * log.cpp
 *
 *  Created on: Oct 29, 2014
 *      Author: thallock
 */


#include "logger.h"

#include <fstream>

namespace
{
	std::ofstream outfile{"output/log.txt"};
}

std::ostream& log()
{
	return std::cout;
}

std::ostream& err()
{
	return std::cerr;
}
