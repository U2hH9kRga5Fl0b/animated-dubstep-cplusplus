/*
 * algo.cpp
 *
 *  Created on: Nov 5, 2014
 *      Author: thallock
 */


#include "insight/algo.h"

#include "assignment.h"
#include "interhub_summary.h"


// For every association of requests to depots
// Find a matching of pickups to delivers
// print out the inter-hub travel


// can we assume times are constant
// does hauling different sizes cost more

// are the a difference between robust solutions and not
//            in terms of adding/removing stops
// what if dumpsters did not have to match
// can we add time-windows?
// varying wait times at landfills



Solution* do_something(City* city)
{
	pairing_info *info = new_pairing_info(city);

	insight_state state{info};
	state.fast_match();

	inter_hub_travel summary{state};
	log() << summary << std::endl;

	Solution* assignment = assign_insight(&state);

	delete info;

	return assignment;
}
