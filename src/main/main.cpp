/*
 * main.cpp
 *
 *  Created on: Oct 24, 2014
 *      Author: thallock
 */

#include "main/global.h"

int main(int argc, char **argv)
{
	// TODO:
	// Enforce the inventories: especially within the codon search
	// the codon search should handle sub-paths of zero length
	// the codon search should handle fixing up each potential codon swap


	// there is still a bug with the nearest one...
	srand(5000007);

	parse_args(argc, argv);

//	void show_path();
//	show_path();
	solve();
	std::cout << "done" << std::endl;
}

