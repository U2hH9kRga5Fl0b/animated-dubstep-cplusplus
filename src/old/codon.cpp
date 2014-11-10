/*
 * codon.cpp
 *
 *  Created on: Oct 25, 2014
 *      Author: thallock
 */

#if 0
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



class codon
{
public:
	truck_state start;
	truck_state stop;

	bool accessible[3];

	int driver, begin, end;

	codon() : start{}, stop{}, driver{-1}, begin{-1}, end{-1} {}

	codon(const Solution* sol, int dri, int b, int e) :
			start { sol->get_in_state(dri, b) }, stop { sol->get_out_state(dri, e) },
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


class ins_point
{
	public:
	truck_state start, stop;
	int driver, ins_after;

	ins_point() :
		start{-1},
		stop{-1},
		driver{-1},
		ins_after{INT_MAX} {}
	ins_point(const Solution* sol, int dri, int b) :
		start {sol->get_in_state(dri, b+1)},
		stop{sol->get_out_state(dri, b)},
		driver{dri},
		ins_after{b} {}
	~ins_point() {};
};




int get_time_delta(const Solution* sol, const codon& c1, const codon& c2)
{
	int prevc1 = c1.begin-1 >= 0 ? sol->get_action_index(c1.driver, c1.begin-1) : START_ACTION_INDEX;
	int firsc1 = sol->get_action_index(c1.driver, c1.begin);
	int lastc1 = sol->get_action_index(c1.driver, c1.end);
	int nextc1 = c1.end+1 < sol->get_number_of_stops(c1.driver) ? sol->get_action_index(c1.driver, c1.end+1) : END_ACTION_INDEX;

	int prevc2 = c2.begin-1 >= 0 ? sol->get_action_index(c2.driver, c2.begin-1) : START_ACTION_INDEX;
	int firsc2 = sol->get_action_index(c2.driver, c2.begin);
	int lastc2 = sol->get_action_index(c2.driver, c2.end);
	int nextc2 = c2.end+1 < sol->get_number_of_stops(c2.driver) ? sol->get_action_index(c2.driver, c2.end+1) : END_ACTION_INDEX;

	if (c1.driver == c2.driver)
	{
		if (c1.end + 1 == c2.begin)
		{
			int oldcost = city->get_time_to(prevc1, firsc1) + city->get_time_to(lastc1, firsc2) + city->get_time_to(lastc2, nextc2);
			int newcost = city->get_time_to(prevc1, firsc2) + city->get_time_to(lastc2, firsc1) + city->get_time_to(lastc1, nextc2);
			return newcost - oldcost;
		}
		else if (c2.end + 1 == c1.begin)
		{
			int oldcost = city->get_time_to(prevc2, firsc2) + city->get_time_to(lastc2, firsc1) + city->get_time_to(lastc1, nextc1);
			int newcost = city->get_time_to(prevc2, firsc1) + city->get_time_to(lastc1, firsc2) + city->get_time_to(lastc2, nextc1);
			return newcost - oldcost;
		}
	}

	const City* city = sol->get_city();
	int oldcost = city->get_time_to(prevc1, firsc1) + city->get_time_to(lastc1, nextc1) + city->get_time_to(prevc2, firsc2) + city->get_time_to(lastc2, nextc2);
	int newcost = city->get_time_to(prevc1, firsc2) + city->get_time_to(lastc2, nextc1) + city->get_time_to(prevc2, firsc1) + city->get_time_to(lastc1, nextc2);

	return newcost - oldcost;
}

int get_time_delta(const Solution* sol, const codon& c1, const ins_point& c2)
{
	int prevc1 = c1.begin-1 >= 0 ? sol->get_action_index(c1.driver, c1.begin-1) : START_ACTION_INDEX;
	int firsc1 = sol->get_action_index(c1.driver, c1.begin);
	int lastc1 = sol->get_action_index(c1.driver, c1.end);
	int nextc1 = c1.end+1 < sol->get_number_of_stops(c1.driver) ? sol->get_action_index(c1.driver, c1.end+1) : END_ACTION_INDEX;

	int prevc2 = c2.ins_after >= 0 ? sol->get_action_index(c2.driver, c2.ins_after) : START_ACTION_INDEX;
	int nextc2 = c2.ins_after + 1 < sol->get_number_of_stops(c2.driver) ? sol->get_action_index(c2.driver, c2.ins_after+1) : END_ACTION_INDEX;

	if (c1.driver == c2.driver)
	{
		if (c1.begin + 1 == c2.ins_after || c1.end + 1 == c2.ins_after)
		{
			return 0;
		}
	}

	const City* city = sol->get_city();
	int oldcost = city->get_time_to(prevc1, firsc1) + city->get_time_to(lastc1, nextc1) + city->get_time_to(prevc2, nextc2);
	int newcost = city->get_time_to(prevc2, firsc1) + city->get_time_to(lastc1, nextc2) + city->get_time_to(prevc1, nextc1);

	return newcost - oldcost;
}

#define SUPER_DEBUG (DEBUG && 1)

bool find_best_codon(const Solution *solution, const codon& c, search_method m)
{
	int num_drivers = solution->get_num_drivers();

	std::set<int>* startstops = new std::set<int>[num_drivers];
	std::set<int>* stopstops  = new std::set<int>[num_drivers];
	std::set<int>* inserts    = new std::set<int>[num_drivers];

	for (int d = 0; d < num_drivers; d++)
	{
		int len = solution->get_number_of_stops(d);
		for (int s = 0; s < len; s++)
		{
			if (solution->get_city()->action(solution->get_action_index(d, s)).exit_state == c.stop)
			{
				stopstops[d].insert(s);
			}
			if (solution->get_city()->action(solution->get_action_index(d, s)).entr_state == c.start)
			{
				startstops[d].insert(s);
			}
			// miss the negative one right now ?
			if (solution->get_city()->action(solution->get_action_index(d, s)).exit_state == c.start && solution->get_city()->action(solution->get_action_index(d, s+1)).entr_state == c.stop
					&& (d != c.driver || s + 1 < c.begin || c.end + 1 < s))
			{
				inserts[d].insert(s);
			}
		}
	}


	bool found_improvement = false;
	bool is_codon;
	codon bestcodon;
	ins_point best_ins;
	int best_improvement = 0;

	for (int driver = 0; driver < num_drivers; driver++)
	{
		if (!c.accessible[city->trucks[driver]])
		{
			continue;
		}

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
				if (!alternative.accessible[city->trucks[c.driver]])
				{
					continue;
				}

				int improvement = get_time_delta(solution, c, alternative);
				if (improvement >= best_improvement)
				{
					continue;
				}

				found_improvement = true;
				bestcodon = alternative;
				best_improvement = improvement;
				is_codon = true;

				if (!SUPER_DEBUG)
				{
					continue;
				}

				Solution other { *solution };
				int old = other.sum_all_times();
				other.exchange(c.driver, c.begin, c.end, alternative.driver, alternative.begin, alternative.end);

				int n = other.sum_all_times();
				int ca = old + improvement;

				if (n == ca)
				{
					continue;
				}

				err() << "actual: " << n - old << std::endl;
				err() << "calculated: " << improvement << std::endl;

				err() << n << ", " << ca << std::endl;
				err() << "mis-calculated improvement." << std::endl;
				err() << "old=" << old << std::endl;
				err() << "improvement=" << improvement << std::endl;
				err() << "new=" << other.sum_all_times() << std::endl;
				trap();
			}
		}

		end = inserts[driver].end();
		for (auto it = inserts[driver].begin(); it != end; ++it)
		{
			int b = (*it);

			ins_point ip{solution, driver, b};

			int improvement = get_time_delta(solution, c, ip);
			if (improvement >= best_improvement)
			{
				continue;
			}

			is_codon = false;
			found_improvement = true;
			best_ins = ip;
			best_improvement = improvement;

			if (!SUPER_DEBUG)
			{
				continue;
			}

			Solution other { *solution };
			int old = other.sum_all_times();
			other.insert_after(c.driver, c.begin, c.end, ip.driver, ip.ins_after);

			int n = other.sum_all_times();
			int ca = old + improvement;

			if (n == ca)
			{
				continue;
			}

			err() << "actual: " << n - old << std::endl;
			err() << "calculated: " << improvement << std::endl;

			err() << n << ", " << ca << std::endl;
			err() << "mis-calculated improvement." << std::endl;
			err() << "old=" << old << std::endl;
			err() << "improvement=" << improvement << std::endl;
			err() << "new=" << other.sum_all_times() << std::endl;
			trap();
		}
	}

	delete[] startstops;
	delete[] stopstops;
	delete[] inserts;

	if (found_improvement)
	{
		if (is_codon)
		{
			solution->exchange(c.driver, c.begin, c.end, bestcodon.driver, bestcodon.begin, bestcodon.end);
		}
		else
		{
			solution->insert_after(c.driver, c.begin, c.end, best_ins.driver, best_ins.ins_after);
		}
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

		int idx1 = rand() % (len-1);
		int idx2 = idx1 + (rand() % (len - idx1));

		if (find_best_codon(solution, codon{solution, driver, idx1, idx2}, LINEAR))
		{
			count = 0;
		}
	}
}


void ensure_local_minima(Solution* solution)
{
	int num_drivers = solution->get_num_drivers();
	bool improvement = true;
	while (improvement)
	{
		improvement = false;
		for (int d = 0; d < num_drivers && !improvement; d++)
		{
			int len = solution->get_number_of_stops(d);
			// need to improve these bounds...
			for (int s1 = 0;  s1 < len && !improvement; s1++)
			for (int s2 = s1; s2 < len && !improvement; s2++)
				improvement |= find_best_codon(solution, codon{solution, d, s1, s2}, LINEAR);
		}
	}
}

#endif
