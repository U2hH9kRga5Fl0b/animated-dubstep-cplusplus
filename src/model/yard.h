/*
 * yard.h
 *
 *  Created on: Oct 24, 2014
 *      Author: thallock
 */

#ifndef YARD_H_
#define YARD_H_

#include <climits>

class yard
{
public:
	yard(int loc) : capacity{INT_MAX}, location{loc} {initial[0]=initial[1]=initial[2]=initial[3] = 10; }
	~yard() {}

	int capacity;
	int initial[4];
	int location;

	friend std::ostream& operator<<(std::ostream& out, const yard& a);
};



#endif /* YARD_H_ */
