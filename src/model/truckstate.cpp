/*
 * truckstate.cpp
 *
 *  Created on: Nov 1, 2014
 *      Author: thallock
 */

#include "model/truckstate.h"

#include <sstream>


std::string get_truck_state_desc(truck_state state)
{
	std::stringstream out;
	switch(state & TRUCK_STATE_MASK)
	{
	case TRUCK_STATE_NONE:
		out << "none";
		break;
	case TRUCK_STATE_EMPTY:
		out << "empty";
		break;
	case TRUCK_STATE_FULL:
		out << "full ";
		break;
	default:
		trap();
	}
	out << "(";
	switch(state & 0xff)
	{
	case 0:
		out << " 0";
		break;
	case 6:
		out << " 6";
		break;
	case 9:
		out << " 9";
		break;
	case 12:
		out << "12";
		break;
	case 16:
		out << "16";
		break;
	default:
		trap();
	}
	out << ")";
	return out.str();
}




