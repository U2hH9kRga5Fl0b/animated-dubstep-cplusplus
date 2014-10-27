/*
 * get_inters.cpp
 *
 *  Created on: Oct 25, 2014
 *      Author: thallock
 */

#include "seed/random.h"

#include <set>

#include <algorithm>

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


struct altern
{
	int poss;
	int time;

	bool operator<(const altern& other) const
	{
		return time < other.time;
	}

	bool operator==(const altern& other) const
	{
		return other.poss == poss;
	}
};

bool backtrack(stuff& stuff)
{
	if (stuff.cdepth + 1 >= stuff.outlen)
	{
		return false;
	}

	bool foundapath = false;


	std::set<altern> nexts;

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
			continue;
		}
		for (int i = 0; i <= stuff.cdepth; i++)
		{
			if (stuff.cinters[i] == poss && stuff.city->get_action(stuff.cinters[i]).value)
			{
				continue;
			}
		}

		if (		stuff.cdepth >= 0 &&
				stuff.city->get_action(stuff.cinters[stuff.cdepth]).op == Unstore &&
				stuff.city->get_action(poss).op == Store
#if 0
				&& stuff.city->get_action(stuff.times[stuff.cdepth]).location == stuff.city->get_action(poss).location
#endif
			)
		{
			// skip these for now...
			continue;
		}


		altern a;
		a.poss = poss;
		a.time = (stuff.cdepth == 0 ? 0 : stuff.times[stuff.cdepth]) + stuff.city->get_time_to(lastaction, poss);

		if (stuff.city->get_action(poss).value && a.time < stuff.besttime)
		{
			foundapath = true;
			stuff.besttime = a.time;
			for (int i = 0; i <= stuff.cdepth; i++)
			{
				stuff.best[i] = stuff.cinters[i];
			}
			stuff.best[stuff.cdepth+1] = poss;
			continue;
		}

		nexts.insert(a);
	}

	if (nexts.size() == 0)
	{
		return foundapath;
	}

	if (nexts.begin()->time > nexts.rbegin()->time)
	{
		auto end = nexts.end();
		for (auto it = nexts.begin(); it != end; ++it)
		{
			const altern& a = (*it);
			std::cout << a.time << std::endl;
		}

		std::cerr << "not sorted!" << std::endl;
		trap();
	}

	auto end = nexts.end();
	for (auto it = nexts.begin(); it != end; ++it)
	{
		const altern& a = (*it);

		if (a.time >= stuff.besttime)
		{
			continue;
		}

		stuff.cdepth++;
		stuff.cinters[stuff.cdepth] = a.poss;
		stuff.times[stuff.cdepth] = a.time;
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
