/*
 * main.cpp
 *
 *  Created on: Nov 2, 2014
 *      Author: thallock
 */

#include "direction/directionsdb.h"

#include "viz/cvvizualizer.h"

void show_path()
{
	directions_db db { "/work/potential-nemesis/python/exparse" };
	std::cout << db << std::endl;


	cvvizualizer viz{"show it to me."};
	viz.show(db.all_directions.front());
	viz.pause();
}

