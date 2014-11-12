/*
 * main.cpp
 *
 *  Created on: Oct 24, 2014
 *      Author: thallock
 */

#include "main/global.h"






#include "insight/algo.h"
#include "c/hungarian.h"

int main(int argc, char **argv)
{
// TODO:
//	implement maxtime
//	implement path relinking
//	finish matching
//
//	implement applying max of several possible moves
//	debug why one bad request is not moved
//	implement smart operation finder
//	implement exchanges within single path
//
//	combine both exchange and reschedule
//
//	reimplement staging area capacity and truck types...




	// there is still a bug with the nearest one...
	srand(5000007);

	parse_args(argc, argv);

	delete do_something(city);

	viewer.pause();

//	show_robustness();
//	solve();


	std::cout << "done" << std::endl;
}

