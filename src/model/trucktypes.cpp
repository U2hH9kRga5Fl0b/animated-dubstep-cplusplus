/*
 * trucktypes.cpp
 *
 *  Created on: Oct 24, 2014
 *      Author: thallock
 */

#include "model/trucktypes.h"

#include "common.h"

std::string truck_name(truck_types t)
{
	switch (t)
	{
	case small:
		return "small";
	case medium:
		return "medium";
	case large:
		return "large";

	}
	std::cerr << "bad truck type " << t << std::endl;
	exit(-1);
	return "bad truck type";
}
