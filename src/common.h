/*
 * common.h
 *
 *  Created on: Oct 24, 2014
 *      Author: thallock
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <iostream>
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
		std::cerr << "bad indices: " << si << ", " << mi << ", " << bi << std::endl;   \
		trap();                                                                        \
	}                                                                                      \
} while(0)




#define TIME_IN_A_DAY (60*60*12 * 5)




#endif /* COMMON_H_ */
