/*
 * main.cpp
 *
 *  Created on: Oct 24, 2014
 *      Author: thallock
 */

#include "main/main.h"

int main(int argc, char **argv)
{
	srand(5000000);
	parse_args(argc, argv);
	solve();
}

