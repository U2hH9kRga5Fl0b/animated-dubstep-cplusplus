/*
 * picture_bounds.h
 *
 *  Created on: Nov 6, 2014
 *      Author: thallock
 */

#ifndef PICTURE_BOUNDS_H_
#define PICTURE_BOUNDS_H_


#include <cfloat>



#define INCREASE(x)                      \
do {                                     \
	if ((x) == 0.0)                  \
	{                                \
		x = 1;                   \
	}                                \
	else if ((x) < 0.0)              \
	{                                \
		x = .9 * x;              \
	}                                \
	else                             \
	{                                \
		x = 1.1 * x;             \
	}                                \
} while(0)

#define DECREASE(x)                      \
do {                                     \
	if ((x) == 0.0)                  \
	{                                \
		x = 1;                   \
	}                                \
	else if ((x) < 0.0)              \
	{                                \
		x = 1.1 * x;             \
	}                                \
	else                             \
	{                                \
		x = .9 * x;              \
	}                                \
} while(0)



#endif /* PICTURE_BOUNDS_H_ */
