/*
 * combination.h
 *
 *  Created on: Nov 18, 2014
 *      Author: thallock
 */

#ifndef COMBINATION_H_
#define COMBINATION_H_

#include "common.h"

struct combination
{
	int n, k;
	int *current;

	bool repeats;

	combination(int n, int k, bool repeats=false);
	~combination();

	bool increment();

	friend std::ostream& operator<<(std::ostream& out, const combination& comb);
};


class combination_iterator
{
		combination* comb;
		int cur_k;
		int max_k;

public:
		combination_iterator(int n, int m_k);
		~combination_iterator();

		const combination& current() const;
		bool increment();

		friend std::ostream& operator<<(std::ostream& out, const combination_iterator& comb)
		{
			return out << comb.current();
		}
};



#endif /* COMBINATION_H_ */
