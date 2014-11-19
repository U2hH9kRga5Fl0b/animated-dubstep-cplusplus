/*
 * combination.cpp
 *
 *  Created on: Nov 18, 2014
 *      Author: thallock
 */

#include "combination.h"

combination::combination(int n_, int k_, bool repeats_) :
	n{n_},
	k{k_},
	current{new int[k]},
	repeats{repeats_}
{
	for (int i = 0; i < k; i++)
		current[i] = 0;
}

combination::~combination()
{
	delete[] current;
}

bool combination::increment()
{
	const bool r = repeats;

	int first_to_increment;
	for (first_to_increment = k - 1; ; first_to_increment--)
	{
		if (first_to_increment < 0)
			return false;

		if (r)
		{
			if (current[first_to_increment] < n)
				break;
		}
		else
		{
			if (current[first_to_increment] < n-1 - (k-1 - first_to_increment))
				break;
		}
	}
	current[first_to_increment]++;

	while (++first_to_increment < k)
		current[first_to_increment] = r ? 0 : (current[first_to_increment-1]+1);

	return true;
}



std::ostream& operator<<(std::ostream& out, const combination& comb)
{
	out << "[";
	for (int i = 0; i < comb.k; i++)
	{
		out << std::setw(5) << comb.current[i] << " ";
	}
	return out << "]";
}
