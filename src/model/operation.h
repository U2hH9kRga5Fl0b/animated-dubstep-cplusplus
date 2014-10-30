/*
 * operation.h
 *
 *  Created on: Oct 24, 2014
 *      Author: thallock
 */

#ifndef OPERATION_H_
#define OPERATION_H_

#include "common.h"

extern bool op_follows_op[6][6];

typedef enum
{
	Pickup   = 0,
	Dropoff  = 1,
	Store    = 2,
	Unstore  = 3,
	Dump     = 4,
	Replace  = 5,
} operation;

char operation_to_char(const operation o);

#endif /* OPERATION_H_ */
