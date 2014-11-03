/*
 * truckstate.h
 *
 *  Created on: Nov 1, 2014
 *      Author: thallock
 */

#ifndef TRUCKSTATE_H_
#define TRUCKSTATE_H_

#include "model/city.h"

#define TRUCK_STATE_FULL  0x00000100
#define TRUCK_STATE_EMPTY 0x00000200
#define TRUCK_STATE_NONE  0x00000400
#define TRUCK_STATE_MASK  0x0000ff00

typedef int truck_state;

truck_state get_truck_out_state(const City* city, int action);
truck_state get_truck_in_state(const City* city, int action);
std::string get_truck_state_desc(truck_state state);

#endif /* TRUCKSTATE_H_ */
