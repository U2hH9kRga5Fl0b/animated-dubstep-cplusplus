/*
 * truckstate.h
 *
 *  Created on: Nov 1, 2014
 *      Author: thallock
 */

#ifndef TRUCKSTATE_H_
#define TRUCKSTATE_H_

#include "model/dumpstersize.h"

#include "common.h"

#define TRUCK_STATE_FULL  0x00000100
#define TRUCK_STATE_EMPTY 0x00000200
#define TRUCK_STATE_NONE  0x00000400

#define TRUCK_STATE_MASK  0x0000ff00
#define TRUCK_SIZE_MASK   0x000000ff

typedef int truck_state;

inline dumpster_size get_state_size(truck_state s) { return (dumpster_size) (s & TRUCK_SIZE_MASK); }
inline bool state_is_full(truck_state s) { return s & TRUCK_STATE_FULL; }
inline bool state_is_empty(truck_state s) { return s & TRUCK_STATE_EMPTY; }
inline bool state_has_no_dumpster(truck_state s) { return s & TRUCK_STATE_NONE; }

std::string get_truck_state_desc(truck_state state);

#endif /* TRUCKSTATE_H_ */
