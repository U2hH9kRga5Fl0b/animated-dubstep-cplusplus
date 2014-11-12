/*
 * insight_sate.cpp
 *
 *  Created on: Nov 5, 2014
 *      Author: thallock
 */

#include "insight/insight_state.h"


insight_state::insight_state(const pairing_info* info_) :
	info{info_}
{
	for (int s = 0; s < 4; s++)
	{
		deliver_depots[s] = new int[info->deliver_lens[s]];
		delivers_to_pickups[s] = new int[info->deliver_lens[s]];
		pickup_depots[s] = new int[info->pickup_lens[s]];

		for (int i = 0; i < info->deliver_lens[s]; i++)
		{
			deliver_depots[s][i] = info->get_nth_closest_deliver_depot(s, i, 0);
			delivers_to_pickups[s][i] = -1;
		}
		for (int i = 0; i < info->pickup_lens[s]; i++)
			pickup_depots[s][i] = info->get_nth_closest_pickup_depot(s, i, 0);
	}
}

insight_state::~insight_state()
{
	for (int s = 0; s < 4; s++)
	{
		delete[] deliver_depots[s];
		delete[] pickup_depots[s];
		delete[] delivers_to_pickups[s];
	}
}

