/*
 * insight_sate.cpp
 *
 *  Created on: Nov 5, 2014
 *      Author: thallock
 */

#include "insight/insight_state.h"

insight_state::insight_state(const pairing_info* info_) :
	info{info_},
	deliver_depots {new int[info->num_delivers]},
	pickup_depots {new int[info->num_pickups]},
	delivers_to_pickups {new int[info->num_delivers]}
{
	for (int i = 0; i < info->num_delivers; i++)
		deliver_depots[i] = info->deliver_depots.at(i, 0);
	for (int i = 0; i < info->num_delivers; i++)
		delivers_to_pickups[i] = -1;
	for (int i = 0; i < info->num_pickups; i++)
		pickup_depots[i] = info->pickup_depots.at(i, 0);
}

insight_state::~insight_state()
{
	delete[] deliver_depots;
	delete[] pickup_depots;
	delete[] delivers_to_pickups;
}

void insight_state::fast_match()
{
	if (0)
	{
		// pick the shortest first
		for (int i = 0; i < 4; i++)
		{
			bool *already_pickedup = new bool[info->num_pickups];
			for (int i = 0; i < info->num_pickups; i++)
				already_pickedup[i] = false;

			for (int i = 0; i < 4; i++)
			{
				int dbegin = info->first_deliver_of_size[i];
				int dend   = i==3 ? info->num_delivers : info->first_deliver_of_size[i+1];
				int pbegin = info->first_pickup_of_size[i];
				int pend   = i==3 ? info->num_pickups : info->first_pickup_of_size[i+1];

#if 0
				for (int d = dbegin; d < dend; d++)
					for (int p = pbegin; p < pend; p++)
#endif

			}
			delete[] already_pickedup;
		}
	}
	else
	{
		int starts[4] = {info->first_deliver_of_size[0],
				info->first_deliver_of_size[1],
				info->first_deliver_of_size[2],
				info->first_deliver_of_size[3]};
		int ends[4] = { info->first_deliver_of_size[1],
				info->first_deliver_of_size[2],
				info->first_deliver_of_size[3],
				info->num_delivers};

		bool *already_pickedup = new bool[info->num_pickups];
		for (int i = 0; i < info->num_pickups; i++)
			already_pickedup[i] = false;

		for (int i = 0; i < 4; i++)
		for (int d = starts[i]; d < ends[i]; d++)
		{
			int deliver = info->deliver_actions[d];
			int pickupIdx = -1;
			int cost = INT_MAX;

			int pstart = info->first_pickup_of_size[i];
			int pend = i == 3 ? info->num_pickups : info->first_pickup_of_size[i+1];
			for (int j = pstart; j < pend; j++)
			{
				if (already_pickedup[j])
					continue;
				int a = info->d2p.at(deliver, info->pickup_actions[j]);
				if (a < cost)
				{
					cost = a;
					pickupIdx = j;
				}
			}

			if (pickupIdx < 0)
			{
				delivers_to_pickups[d] = -1;
				continue;
			}

			int pickup = info->pickup_actions[pickupIdx];
			if (cost >= info->city->get_time_to(deliver, deliver_depots[i]) + info->city->get_time_to(pickup, pickup_depots[pickup]))
			{
				delivers_to_pickups[d] = -1;
				continue;
			}

			delivers_to_pickups[d] = pickup;
			already_pickedup[pickupIdx] = true;
		}

		delete[] already_pickedup;
	}
}
