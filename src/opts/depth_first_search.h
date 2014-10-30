/*
 * depth_first_search.h
 *
 *  Created on: Oct 29, 2014
 *      Author: thallock
 */

#ifndef DEPTH_FIRST_SEARCH_H_
#define DEPTH_FIRST_SEARCH_H_

#include "model/solution.h"

#include <vector>

class insertion
{
public:
	int driver, start;
	std::vector<int> subpath;

	insertion(int driver_, int start_) : driver{driver_}, start{start_} {}
	~insertion() {}

	void apply(Solution* solution)
	{
		int num_stops = solution->get_number_of_stops(driver);
		if (start < num_stops)
		{
			std::cerr << *solution << std::endl;
			std::cerr << "not implemented!" << std::endl;
			trap();
		}
		int size = subpath.size();
		for (int i = 0; i < size; i++)
		{
			solution->append(driver, start + i, subpath.at(i));
		}
	}
};

bool search_for_path(Solution* solution, int end_index, int maxdepth, insertion& ins);

#endif /* DEPTH_FIRST_SEARCH_H_ */
