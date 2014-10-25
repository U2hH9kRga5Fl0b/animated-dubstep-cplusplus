/*
 * landfill.h
 *
 *  Created on: Oct 24, 2014
 *      Author: thallock
 */

#ifndef LANDFILL_H_
#define LANDFILL_H_

class landfill
{
public:
	landfill(int loc);
	~landfill();

	long waittime;
	int location;
};

#endif /* LANDFILL_H_ */
