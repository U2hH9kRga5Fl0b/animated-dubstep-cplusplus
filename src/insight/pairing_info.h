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

	int num_pickups;
	int *pickup_actions;
	int first_pickup_of_size[4];
	intarray pickup_depots;

	int num_delivers;
	int *deliver_actions;
	int first_deliver_of_size[4];
	intarray deliver_depots;

	intarray depot_2_depot;

	intarray d2p;


	pairing_info(const City* city, int d, int p, int ndpts);
	~pairing_info();

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

};

pairing_info* new_pairing_info(const City* city);

#endif /* PAIRING_INFO_H_ */
