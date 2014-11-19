/*
 * combination.h
 *
 *  Created on: Nov 18, 2014
 *      Author: thallock
 */

#ifndef COMBINATION_H_
#define COMBINATION_H_

#include "common.h"

class combination
{
	int n, k;
	int *current;

	bool repeats;
public:
	combination(int n, int k, bool repeats=false);
	~combination();

	bool increment();

	friend std::ostream& operator<<(std::ostream& out, const combination& comb);

};

#endif /* COMBINATION_H_ */
