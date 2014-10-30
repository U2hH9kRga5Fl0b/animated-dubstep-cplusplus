/*
 * nearest_valuable.cpp
 *
 *  Created on: Oct 25, 2014
 *      Author: thallock
 */

#include "opts/depth_first_search.h"

#include <set>

namespace
{

#define MAX_SEARCH_DEPTH 10

typedef struct
{
	// global
	bool *already_serviced;
	const City* city;
	int firstaction;
	int lastaction;
	int maxdepth;

	// best
	int best[MAX_SEARCH_DEPTH];
	int bestlen;
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
	if (stuff.cdepth + 1 >= stuff.maxdepth)
	{
		return false;
	}

	bool foundapath = false;


	std::set<altern> nexts;

	int lastaction = stuff.cdepth == -1 ? stuff.firstaction : stuff.cinters[stuff.cdepth];
	for (int p = 0; p < stuff.city->possibles.cols(); p++)
	{
		int poss = stuff.city->possibles.at(lastaction, p);
		if (poss < 0)
		{
			break;
		}
		if (stuff.already_serviced[poss])
		{
			continue;
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

		bool is_destination = stuff.lastaction == -1 ? stuff.city->get_action(poss).value : poss == lastaction;

		if (is_destination && a.time < stuff.besttime)
		{
			foundapath = true;
			stuff.besttime = a.time;
			for (int i = 0; i <= stuff.cdepth; i++)
			{
				stuff.best[i] = stuff.cinters[i];
			}
			stuff.best[stuff.cdepth+1] = poss;
			stuff.bestlen = stuff.cdepth + 2;
			continue;
		}

		nexts.insert(a);
	}

	if (nexts.size() == 0)
	{
		return foundapath;
	}

	auto end = nexts.end();
	for (auto it = nexts.begin(); it != end; ++it)
	{
		const altern& a = (*it);

		if (a.time >= stuff.besttime)
		{
			continue;
		}

		bool has_value = stuff.city->get_action(a.poss).value;

		stuff.cdepth++;
		stuff.cinters[stuff.cdepth] = a.poss;
		stuff.times[stuff.cdepth] = a.time;
		if (has_value) stuff.already_serviced[a.poss] = true;

		foundapath |= backtrack(stuff);

		if (has_value) stuff.already_serviced[a.poss] = false;
		stuff.cdepth--;
	}

	return foundapath;
}

}

bool search_for_path(Solution* solution, int stop, int maxdepth, insertion& ins)
{
	if (maxdepth > MAX_SEARCH_DEPTH)
	{
		err() << "This is not supported." << std::endl;
		trap();
	}

	if (ins.start == 0)
	{
		trap();
	}

	stuff s;
	s.already_serviced = new bool[solution->get_city()->num_actions];
	for (int i = 0; i < solution->get_city()->num_actions; i++)
	{
		s.already_serviced[i] = (i < ins.start || i > stop) && solution->already_serviced(i);
	}
	s.firstaction = solution->get_action_index(ins.driver, ins.start - 1);
	if (stop < 0)
	{
		s.lastaction = -1;
	}
	else
	{
		s.lastaction = solution->get_action_index(ins.driver, stop);
	}
	s.city=solution->get_city();
	s.maxdepth = maxdepth;
	s.bestlen = -1;
	s.besttime = INT_MAX;
	s.cdepth = -1;

	if (!backtrack(s))
	{
		return false;
	}

	ins.subpath.clear();
	for (int i = 0; i < s.bestlen; i++)
	{
		ins.subpath.push_back(s.best[i]);
	}

	return true;
}
