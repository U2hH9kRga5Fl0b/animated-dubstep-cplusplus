/*
 * reduction.h
 *
 *  Created on: Nov 4, 2014
 *      Author: thallock
 */

#ifndef REDUCTION_H_
#define REDUCTION_H_

#include "model/city.h"
#include "intarray.h"

#include <list>

class pairing_info
{
private:
	int num_pickups;
	int *pickup_actions;

	int num_delivers;
	int *deliver_actions;

	intarray d2p;

public:
	pairing_info(City* city);
	~pairing_info();
};

class inter_hub_travel
{
	intarray num_from_to;

};




class reduction
{
public:
	reduction(const City* city);
	~reduction();

	intarray possible_depots;

	int *associated_depots;
	int *delivers_to_pickups;
};


// For every association of requests to depots
// Find a matching of pickups to delivers
// print out the inter-hub travel










#endif /* REDUCTION_H_ */
