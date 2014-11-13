/*
 * pairing_info.h
 *
 *  Created on: Nov 5, 2014
 *      Author: thallock
 */

#ifndef PAIRING_INFO_H_
#define PAIRING_INFO_H_

#include "model/city.h"

#include "intarray.h"


inline int d2i(truck_state state)
{
	switch (state & TRUCK_SIZE_MASK)
	{
		case none   : trap(); return -1300;
		case six    : return 0;
		case nine   : return 1;
		case twelve : return 2;
		case sixteen: return 3;
	}
	trap();
	return -1300;
}



struct subarray
{
	int begin;
	int end;

	subarray() : begin{INT_MAX}, end{-1} {}
};


class pairing_info
{
public:
	const City* city;
	int num_staging_areas;

	int total_num_pickups;
	int pickup_lens;
	int *pickup_actions;
	subarray pickup_sizes[4];


	int total_num_delivers;
	int deliver_lens;
	int *deliver_actions;
	subarray deliver_sizes[4];

	intarray depot_2_depot;

	pairing_info(const City* city);
	~pairing_info();

	int get_nth_closest_deliver_depot(int deliver_index, int n) const;
	int get_nth_closest_pickup_depot(int pickup_index, int n) const;

	int find_deliver_index(int deliver_action) const
	{
		for (int i = 0; i < deliver_lens; i++)
			if (deliver_actions[i] == deliver_action)
				return i;
		trap();
		return -1300;
	}
	int find_pickup_index(int pickup_action) const
	{
		for (int i = 0; i < pickup_lens; i++)
			if (pickup_actions[i] == pickup_action)
				return i;
		trap();
		return -1300;
	}
private:
	int ** closest_delivers;
	int ** closest_pickups;
};

#if 0
	dumpster_size get_deliver_size(int d) const
	{
		INBOUNDS(0, d, num_delivers);
		if (d < first_deliver_of_size[2])
			if (d < first_deliver_of_size[1])
				return six;
			else
				return nine;
		else
			if (d < first_deliver_of_size[3])
				return twelve;
			else
				return sixteen;
	}
	dumpster_size get_pickup_size(int p) const
	{
		INBOUNDS(0, p, num_delivers);
		if (p < first_pickup_of_size[2])
			if (p < first_pickup_of_size[1])
				return six;
			else
				return nine;
		else
			if (p < first_pickup_of_size[3])
				return twelve;
			else
				return sixteen;
	}
	pairing_info* new_pairing_info(const City* city);
#endif

#endif /* PAIRING_INFO_H_ */
