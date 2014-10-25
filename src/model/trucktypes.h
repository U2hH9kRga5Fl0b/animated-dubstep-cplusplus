/*
 * trucktypes.h
 *
 *  Created on: Oct 24, 2014
 *      Author: thallock
 */

#ifndef TRUCKTYPES_H_
#define TRUCKTYPES_H_

#include <string>

typedef enum
{
	small   = 0,
	medium  = 1,
	large   = 2,
} truck_types;

std::string truck_name(truck_types t);

#endif /* TRUCKTYPES_H_ */
