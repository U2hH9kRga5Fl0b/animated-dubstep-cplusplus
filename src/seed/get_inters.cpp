/*
 * get_inters.cpp
 *
 *  Created on: Oct 25, 2014
 *      Author: thallock
 */

#include "seed/random.h"


namespace
{

#define MAX_SEARCH_DEPTH 10

typedef struct
{
	// global
	const Solution* sol;
	const City* city;
	int a1;

	// best
	int *best;
	int outlen;
	int besttime;

	// current
	int cinters[MAX_SEARCH_DEPTH];
	int times[MAX_SEARCH_DEPTH];
	int cdepth;
} stuff;


bool backtrack(stuff& stuff)
{
	if (stuff.cdepth + 1 >= stuff.outlen)
	{
		return false;
	}

//	std::cout << "considering inters: (depth=" << stuff.cdepth << ")" << std::endl;
//	for (int i = 0; i <= stuff.cdepth; i++)
//	{
//		std::cout << stuff.cinters[i] << " " << stuff.city->get_action(stuff.cinters[i]) << "with time " << stuff.times[i] << std::endl;
//	}

	bool foundapath = false;

	int lastaction = stuff.cdepth == -1 ? stuff.a1 : stuff.cinters[stuff.cdepth];
	for (int p = 0; p < stuff.city->possibles.cols(); p++)
	{
		int poss = stuff.city->possibles.at(lastaction, p);
		if (poss < 0)
		{
			break;
		}
		if (stuff.sol->already_serviced(poss))
		{
//			std::cout << "already serviced " << poss << std::endl;
			continue;
		}
		for (int i = 0; i <= stuff.cdepth; i++)
		{
			if (stuff.cinters[i] == poss && stuff.city->get_action(stuff.cinters[i]).value)
			{
				continue;
			}
		}

		stuff.cdepth++;
		stuff.cinters[stuff.cdepth] = poss;
		stuff.times[stuff.cdepth] = (stuff.cdepth == 0 ? 0 : stuff.times[stuff.cdepth-1]) + stuff.city->get_time_to(lastaction, stuff.cinters[stuff.cdepth]);
		if (stuff.times[stuff.cdepth] >= stuff.besttime)
		{
			stuff.cdepth--;
			continue;
		}

		if (stuff.city->get_action(poss).value)
		{
//			std::cout << "Found a path!" << std::endl;
			foundapath = true;
			// we never let ourselves get worse than best time...
			stuff.besttime = stuff.times[stuff.cdepth];
			for (int i = 0; i <= stuff.cdepth; i++)
			{
				stuff.best[i] = stuff.cinters[i];
			}
			stuff.cdepth--;
			continue;
		}

		foundapath |= backtrack(stuff);
		stuff.cdepth--;
	}

	return foundapath;
}

}

/** Just finds the best time between the two actions **/

int get_best_path(const Solution* solution,
		int lastaction,
		int *out, int outlen)
{
	if (outlen > MAX_SEARCH_DEPTH)
	{
		std::cerr << "This is not supported." << std::endl;
		trap();
	}

	stuff s;
	s.sol = solution;
	s.city=solution->c;
	s.a1=lastaction;
	s.best = out;
	s.outlen = outlen;
	s.besttime = INT_MAX;
	s.cdepth = -1;
	if (backtrack(s))
	{
		return s.besttime;
	}
	else
	{
		return -1;
	}
}
