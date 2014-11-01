/*
 * codon.cpp
 *
 *  Created on: Oct 25, 2014
 *      Author: thallock
 */

#include "opts/codon.h"
#include "main/global.h"

#include <set>


namespace
{

// Not used...
typedef enum
{
	RANDOM,
	LINEAR,
} search_method;



class stop_state
{
public:
	int8_t container_state; // -1: none, 0: empty, 1: full
	uint8_t size;           // 0,6,9,12,16
	stop_state() : container_state{-1}, size{255} {}
	stop_state(const Solution* sol, int driver, int stop)
	{
		if (stop < 0)
		{
			container_state = -1;
			size = 0;
			return;
		}
		int idx = sol->get_action_index(driver, stop);
		if (idx < 0)
		{
			container_state = -1; size = 0;
			return;
		}

		const Action& a = sol->get_city()->get_action(idx);
		switch (a.op)
		{
		case Pickup:
		case Replace:
			container_state = 1;
			break;
		case Dropoff:
		case Store:
			container_state = -1;
			break;

		case Unstore:
		case Dump:
			container_state = 0;
			break;
		default:
			trap();
		}
		size = (uint8_t) a.out;
	}
	~stop_state(){}

	friend std::ostream& operator<<(std::ostream& out, const stop_state& state)
	{
		switch(state.container_state)
		{
		case -1:
			out << "none";
			break;
		case 0:
			out << "empty";
			break;
		case 1:
			out << "full";
			break;
		}
		out << "(";
		switch(state.size)
		{
		case 0:
			out << "0";
			break;
		case 6:
			out << "6";
			break;
		case 9:
			out << "9";
			break;
		case 12:
			out << "12";
			break;
		case 16:
			out << "16";
			break;
		default:
			trap();
		}
		out << ")";
		return out;
	}

	bool operator==(const stop_state& other)
	{
		return container_state == other.container_state && size == other.size;
	}
};

class codon
{
public:
	stop_state start;
	stop_state stop;
	bool accessible[3];
	int driver, begin, end;

	codon() : start{}, stop{}, driver{-1}, begin{-1}, end{-1} {}
	codon(const Solution* sol, int dri, int b, int e) :
			start { sol, dri, b }, stop { sol, dri, e },
			driver{dri}, begin{b}, end{e}
	{
		accessible[0] = accessible[1] = accessible[2] = true;
		for (int i = begin+1; i <= end; i++)
		{
			int stop = sol->get_action_index(driver, i);
			const Action& ac = sol->get_city()->get_action(stop);
			for (int j = 0; j < 3; j++)
			{
				accessible[j] &= ac.accessible[j];
			}
		}
	}

	~codon() {}

	bool intersects(const codon& other) const
	{
		return driver == other.driver && (
			(begin <= other.begin && other.begin <= end) ||
			(begin <= other.end   && other.end   <= end) ||
			(other.begin <= begin && begin <= other.end) ||
			(other.begin <= end   && end   <= other.end));
	}
};


void find_compatible_states(Solution* solution, stop_state state, std::set<int> *compats, search_method m)
{
	int num_drivers = solution->get_num_drivers();
	for (int d = 0; d < num_drivers; d++)
	{
		int len = solution->get_number_of_stops(d);
		for (int s = 0; s < len; s++)
		{
			if (stop_state{solution, d, s} == state)
			{
				compats[d].insert(s);
			}
		}
	}
}



int get_time_delta(const Solution* sol, const codon& c1, const codon& c2)
{
	int prevc1 = c1.begin >= 0 ? sol->get_action_index(c1.driver, c1.begin) : START_ACTION_INDEX;
	int firsc1 = sol->get_action_index(c1.driver, c1.begin+1);
	int lastc1 = sol->get_action_index(c1.driver, c1.end);
	int nextc1 = c1.end+1 < sol->get_number_of_stops(c1.driver) ? sol->get_action_index(c1.driver, c1.end+1) : END_ACTION_INDEX;

	int prevc2 = c2.begin >= 0 ? sol->get_action_index(c2.driver, c2.begin) : START_ACTION_INDEX;
	int firsc2 = sol->get_action_index(c2.driver, c2.begin+1);
	int lastc2 = sol->get_action_index(c2.driver, c2.end);
	int nextc2 = c2.end+1 < sol->get_number_of_stops(c2.driver) ? sol->get_action_index(c2.driver, c2.end+1) : END_ACTION_INDEX;

	const City* city = sol->get_city();
	int oldcost = city->get_time_to(prevc1, firsc1) + city->get_time_to(lastc1, nextc1) + city->get_time_to(prevc2, firsc2) + city->get_time_to(lastc2, nextc2);
	int newcost = city->get_time_to(prevc1, firsc2) + city->get_time_to(lastc2, nextc1) + city->get_time_to(prevc2, firsc1) + city->get_time_to(lastc1, nextc2);

	return newcost - oldcost;
}

#define SUPER_DEBUG


bool find_best_codon(Solution *solution, const codon& c, search_method m)
{
	int num_drivers = solution->get_num_drivers();

	std::set<int>* startstops = new std::set<int>[num_drivers];
	std::set<int>* stopstops  = new std::set<int>[num_drivers];

	find_compatible_states(solution, c.start, startstops, m);
	find_compatible_states(solution, c.stop,  stopstops,  m);


	bool found_improvement = false;
	codon bestcodon;
	int best_improvement = 0;

	for (int driver = 0; driver < num_drivers; driver++)
	{
#if ENFORCE_TRUCK_TYPES
				if (!c.accessible[city->trucks[driver]])
				{
					continue;
				}
#endif

		auto end = startstops[driver].end();
		for (auto it = startstops[driver].begin(); it != end; ++it)
		{
			int b = (*it);

			if (driver == c.driver && c.begin < b && c.end > b)
			{
				continue;
			}

			auto secondend = stopstops[driver].end();
			for (auto secondit = stopstops[driver].lower_bound(b); secondit != secondend; ++secondit)
			{
				int e = (*secondit);
				// should check if the paths are equal

				if (e == b)
				{
					continue;
				}

				if (driver == c.driver && c.begin < e && c.end > e)
				{
					continue;
				}

				codon alternative{solution, driver, b, e};
				if (alternative.intersects(c))
				{
					continue;
				}
#if ENFORCE_TRUCK_TYPES
				if (!alternative.accessible[city->trucks[c.driver]])
				{
					continue;
				}
#endif

				int improvement = get_time_delta(solution, c, alternative);
				if (improvement >= best_improvement)
				{
					continue;
				}

				found_improvement = true;
				bestcodon = alternative;
				best_improvement = improvement;
			}
		}
	}

	delete[] startstops;
	delete[] stopstops;


	if (found_improvement)
	{
		solution->exchange(c.driver, c.begin+1, c.end, bestcodon.driver, bestcodon.begin+1, bestcodon.end);
		viewer.show("local search", solution);
	}

	return found_improvement;
}

}


void exchange_subpath_search(Solution* solution, int fail_threshold)
{
	int num_drivers = solution->get_num_drivers();

	int count = 0;
	while (count++ < fail_threshold)
	{
		int driver;
		int len;

		do
		{
			driver = rand() % num_drivers;
			len = solution->get_number_of_stops(driver);
		} while(len == 0);

		int idx1 = -1 + (rand() % len);
		int idx2 = idx1 + 1 + rand() % (len-1 - idx1);

		if (find_best_codon(solution, codon{solution, driver, idx1, idx2}, LINEAR))
		{
			count = 0;
		}
	}
}
