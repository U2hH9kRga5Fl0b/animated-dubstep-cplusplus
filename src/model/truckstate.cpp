/*
 * truckstate.cpp
 *
 *  Created on: Nov 1, 2014
 *      Author: thallock
 */

#include "model/truckstate.h"

#include <sstream>


truck_state get_truck_in_state(const City* city, int action)
{
	if (action == END_ACTION_INDEX)
	{
		return TRUCK_STATE_NONE;
	}

	truck_state ret_val = city->get_action(action).in;
	switch (city->get_action(action).op)
	{
		case Dump:
			ret_val = ret_val | TRUCK_STATE_FULL;
			break;
		case Pickup:
		case Unstore:
			ret_val = ret_val | TRUCK_STATE_NONE;
			break;
		case Replace:
		case Dropoff:
		case Store:
			ret_val = ret_val | TRUCK_STATE_EMPTY;
			break;
		default:
			trap();
	}
	return ret_val;
}

truck_state get_truck_out_state(const City* city, int action)
{
	if (action == START_ACTION_INDEX)
	{
		return TRUCK_STATE_NONE;
	}

	truck_state ret_val = city->get_action(action).out;
	switch (city->get_action(action).op)
	{
		case Pickup:
		case Replace:
			ret_val = ret_val | TRUCK_STATE_FULL;
			break;
		case Dropoff:
		case Store:
			ret_val = ret_val | TRUCK_STATE_NONE;
			break;

		case Unstore:
		case Dump:
			ret_val = ret_val | TRUCK_STATE_EMPTY;
			break;
	default:
		trap();
	}
	return ret_val;
}

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
		out << "full";
		break;
	default:
		trap();
	}
	out << "(";
	switch(state & 0xff)
	{
	case 0:
		out << "0";
		break;
	case 6:
		out << "6";
		break;
	case 9:
		out << "9";
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




