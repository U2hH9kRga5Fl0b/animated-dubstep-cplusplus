/*
 * nearest_valuable.cpp
 *
 *  Created on: Oct 25, 2014
 *      Author: thallock
 */

#include "opts/depth_first_search.h"

#include <set>
#include <algorithm>

namespace
{

#define MAX_SEARCH_DEPTH 10

typedef struct
{
	// global
	int driver;
	bool *already_serviced;
	const City* city;
	int firstaction;
	int maxdepth;

	// best
	int best[MAX_SEARCH_DEPTH];
	int bestlen;
	int besttime;

	// current
	int cinters[MAX_SEARCH_DEPTH];
	int times[MAX_SEARCH_DEPTH];
	int cdepth;

	std::function<bool(const City*, int action)> cri;
} stuff;


class altern
{
public:
	altern() : poss{-1}, time{-1} {};
	~altern(){};
	int poss;
	int time;

	bool operator<(const altern& other) const
	{
		if (time == other.time) return poss > other.poss;
		return time < other.time;
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
		int poss = lastaction >= 0 ? stuff.city->possibles.at(lastaction, p) : p;
		if (poss < 0)
		{
			break;
		}
		if (stuff.already_serviced[poss]
			|| !stuff.city->driver_can_service(stuff.driver, poss))
		{
			continue;
		}

		// need to check inventories...

		if (		stuff.cdepth >= 0 &&
				stuff.city->is_staging_area(stuff.cinters[stuff.cdepth]) &&
				stuff.city->is_staging_area(poss) &&
				stuff.city->get_action(stuff.cinters[stuff.cdepth]).location == stuff.city->get_action(poss).location
			)
		{
			// skip these for now...
			continue;
		}


		altern a;
		a.poss = poss;
		a.time = (stuff.cdepth <= 0 ? 0 : stuff.times[stuff.cdepth]) + stuff.city->get_time_to(lastaction, poss);

		if (stuff.cri(stuff.city, poss))
		{
			if (a.time >= stuff.besttime)
			{
				continue;
			}
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

		nexts.insert(altern{a});
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

		if (a.poss < 0)
		{
			err() << "uhoh" << std::endl;
			trap();
		}

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

bool search_for_path(Solution* solution, int stop, int maxdepth, insertion& ins, std::function<bool(const City*, int action)> cri)
{
	if (maxdepth > MAX_SEARCH_DEPTH)
	{
		err() << "This is not supported." << std::endl;
		trap();
	}

	stuff s;
	s.already_serviced = new bool[solution->get_city()->num_actions];
	for (int i = 0; i < solution->get_city()->num_actions; i++)
	{
		s.already_serviced[i] = (i < ins.start || i > stop) && solution->already_serviced(i);
	}
	s.firstaction = solution->get_action_index(ins.driver, ins.start - 1);
	s.city=solution->get_city();
	s.maxdepth = maxdepth;
	s.bestlen = -1;
	s.besttime = INT_MAX;
	s.cdepth = -1;
	s.cri=cri;
	s.driver = ins.driver;

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
