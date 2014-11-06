/*
 * common.h
 *
 *  Created on: Oct 24, 2014
 *      Author: thallock
 */

#ifndef COMMON_H_
#define COMMON_H_

#include "logger.h"

#include <iomanip>

void trap();

#define DEBUG 1

#define INBOUNDS(x,y,z)                                                                        \
do                                                                                             \
{                                                                                              \
	if (!DEBUG)                                                                            \
	{                                                                                      \
		break;                                                                         \
	}                                                                                      \
	int si=(x),mi=(y),bi=(z);                                                              \
	if (!(si<=mi) || !(mi<bi) || !(bi>si))                                                 \
	{                                                                                      \
		err() << "bad indices: " << si << ", " << mi << ", " << bi << std::endl;       \
		trap();                                                                        \
	}                                                                                      \
} while(0)




#define TIME_IN_A_DAY (60*60*12 * 5)
#define STAGING_AREAS_GO_WITH_LANDFILLS 0


#define TIME_FOR_STAGING_AREA    (( 5 + rand() %  1)*60)
#define TIME_FOR_LANDFILL        ((30 + rand() % 30)*60)
#define TIME_FOR_REQUEST         (( 5 + rand() %  1)*60)



// enforced constraints

// in order to be valid:
//	operation orders
//	start state
//	truck types
//	no repeated stop

// feasible:
//	end state
#define ENFORCE_INVENTORIES      0



// not fully implemented:
#define ENFORCE_TIME_WINDOWS     0








#endif /* COMMON_H_ */
