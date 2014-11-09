/*
 * neighbor.h
 *
 *  Created on: Oct 29, 2014
 *      Author: thallock
 */





#if 0




#ifndef NEIGHBOR_H_
#define NEIGHBOR_H_

#include "model/cost.h"

class neighbor_operation
{
	 neighbor_operation() {}
	~neighbor_operation() {}

	virtual cost get_improvement(const Solution* solution) = 0;
	virtual bool applicable(const Solution* solution)      = 0;
	virtual void apply(Solution* solution)                 = 0;
};

#endif /* NEIGHBOR_H_ */



#endif
