/*
 * reduction.cpp
 *
 *  Created on: Nov 4, 2014
 *      Author: thallock
 */

#include "insight/reduction.h"

#include <algorithm>

namespace
{

struct b4_dpt_cmp
{
	const City* city; int req;
	b4_dpt_cmp(const City* c, int i) : city{c}, req{i} {}
	bool operator()(const int d1, const int d2) const
	{
		return city->get_time_to(d1, req) < city->get_time_to(d2, req);
	}
};
struct aftr_dpt_cmp
{
	const City* city; int req;
	aftr_dpt_cmp(const City* c, int i) : city{c}, req{i} {}
	bool operator()(const int d1, const int d2) const
	{
		return city->get_time_to(d1, req) < city->get_time_to(d2, req);
	}
};

}

reduction::reduction(const City* city) :
	paths {new std::list<stop>[city->num_trucks]},
	possible_depots {city->num_requests, city->num_stagingareas},
	associations {new int[city->num_requests]}
{
	for (int i = 0; i < city->num_requests; i++)
	{
		for (int j = 0; j < city->num_stagingareas; j++)
			possible_depots.at(i, j) = j;

		if(city->get_action(i).entr_state == TRUCK_STATE_NONE)
			std::sort(possible_depots.at(i, 0), possible_depots.at(i+1, 0), aftr_dpt_cmp{city, i + city->first_request_index});
		else
			std::sort(possible_depots.at(i, 0), possible_depots.at(i+1, 0), b4_dpt_cmp{city, i + city->first_request_index});
	}

	for (int i=0;i<city->num_requests;i++)
		associations[i] = 0;
}

reduction::~reduction()
{
	delete[] paths;
	delete[] associations;
}


// can we assume times are constant
// does hauling different sizes cost more

// are the a difference between robust solutions and not
//            in terms of adding/removing stops
// what if dumpsters did not have to match
// can we add time-windows?
// varying wait times at landfills
