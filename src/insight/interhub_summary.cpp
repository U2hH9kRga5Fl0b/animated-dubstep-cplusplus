
/*
 * interhub_summary.cpp
 *
 *  Created on: Nov 5, 2014
 *      Author: thallock
 */

#include "insight/interhub_summary.h"

inter_hub_travel::inter_hub_travel(const insight_state& state) :
		num_from_to { state.info->num_staging_areas, state.info->num_staging_areas }
{
	num_from_to = 0;

	for (int i = 0; i < state.info->deliver_lens; i++)
	{
		if (state.delivers_to_pickups[i] < 0)
			continue;
		num_from_to.at(state.deliver_depots[i], state.pickup_depots[state.delivers_to_pickups[i]])++;
	}

//	auto end = state.inters.end();
//	for (auto it = state.inters.begin(); it != end; ++it)
//		num_from_to.at(it->begin, it->end)++;
}
