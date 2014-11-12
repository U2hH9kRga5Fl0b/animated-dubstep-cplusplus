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

class pairing_info
{
public:
	const City* city;
	int num_staging_areas;

	int total_num_pickups;
	int pickup_lens[4];
	int *pickup_actions[4];

	int total_num_delivers;
	int deliver_lens[4];
	int *deliver_actions[4];

	intarray depot_2_depot;

	pairing_info(const City* city);
	~pairing_info();

	int get_nth_closest_deliver_depot(int dumpster_index, int deliver_index, int n) const;
	int get_nth_closest_pickup_depot(int dumpster_index, int pickup_index, int n) const;

private:
	int **closest_delivers[4];
	int **closest_pickups[4];
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
