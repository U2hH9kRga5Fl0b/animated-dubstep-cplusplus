/*
 * main.cpp
 *
 *  Created on: Oct 24, 2014
 *      Author: thallock
 */

#include "main/global.h"

int main(int argc, char **argv)
{
	srand(5000002);


	void show_path();
	show_path();


	parse_args(argc, argv);
	solve();
	std::cout << "done" << std::endl;
}

