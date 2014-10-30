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
	return outfile;
}

std::ostream& err()
{
	return outfile;
}
