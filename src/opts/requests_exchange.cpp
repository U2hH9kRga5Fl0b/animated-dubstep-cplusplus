/*
 * requests_swap.cpp
 *
 *  Created on: Nov 2, 2014
 *      Author: thallock
 */


#include "opts/requests_exchange.h"

#include "opts/codon.h"
#include "main/global.h"
#include "opts/find_path.h"
#include "opts/requestsubpathiterator.h"

#include <set>

namespace
{

#define LESS_THAN(x,y)          \
do                              \
{                               \
	if (x < y)              \
	{                       \
		break;          \
	}                       \
                                \
	int tmp = x;            \
	x = y;                  \
	y = tmp;                \
} while (0)


int get_random_request(const Solution* solution, int driver, int length, int avoid)
{
	if (length == 0)
	{
		return -1;
	}
	int ndx = rand() % length;

	for (int i = 0; i < length; i++, ndx++)
	{
		if (ndx >= length) ndx = 0;
		if (IS_REQUEST(solution, driver, ndx) && ndx != avoid)
		{
			if (DEBUG && !solution->get_city()->is_request(solution->get_action_index(driver, ndx))) trap();
			return ndx;
		}
	}

	return -1;
}

}


bool apply_random_request_exchange(Solution* solution, int num_attempts, const objective* obj)
{
	const int num_drivers = solution->get_num_drivers();

	int count = 0;
	while (count++ < num_attempts)
	{
		int driver1 = rand() % num_drivers;
		int driver2 = rand() % num_drivers;

		int length1 = solution->get_number_of_stops(driver1);
		int length2 = solution->get_number_of_stops(driver2);

		int begin1, end1;
		int begin2, end2;

		begin1 = get_random_request(solution, driver1, length1, INT_MAX);
		if (begin1 < 0)
		{
			continue;
		}
		end1 = get_random_request(solution, driver1, length1, begin1);
		if (end1 < 0)
		{
			continue;
		}
		LESS_THAN(begin1, end1);

		begin2 = get_random_request(solution, driver2, length2, INT_MAX);
		if (begin1 < 0)
		{
			continue;
		}
		end2 = get_random_request(solution, driver2, length2, begin2);
		if (end2 < 0)
		{
			continue;
		}
		LESS_THAN(begin2, end2);

		if (driver1 == driver2)
		{
			if (begin2 == begin1 || begin2 == end1 || end2 == begin1 || end2 == end1)
			{
				continue;
			}

			// Could probably done more efficiently...
			LESS_THAN(begin1, end1);
			LESS_THAN(begin1, begin2);
			LESS_THAN(begin1, end2);

			LESS_THAN(end1, begin2);
			LESS_THAN(end1, end2);

			LESS_THAN(begin2, end2);
		}


		// This doesn't work on reschedules that are at the end...
		if (consider_reschedule(solution, driver1, begin1, end1, driver2, begin2, obj))
		{
			return true;
		}
		if (consider_exchange(solution, driver1, begin1, end1, driver2, begin2, end2, obj))
		{
			return true;
		}
		else if (count++ >= num_attempts)
		{
			return false;
		}
	} while(false);
	return false;
}




bool apply_first_exchange(Solution* solution, const objective* obj, bool cont)
{
	bool retval = false;

	const int num_drivers = solution->get_num_drivers();
	for (int d1 = 0; d1 < num_drivers; d1++)
	{
		int len1 = solution->get_number_of_stops(d1);
		for (int b1 = 0; b1 < len1; b1++)
		if (IS_REQUEST(solution, d1, b1))
		for (int e1 = b1 + 1; e1 < len1; e1++)
		if (IS_REQUEST(solution, d1, e1))
		for (int d2 = d1; d2 < num_drivers; d2++)
		{
			int len2 = solution->get_number_of_stops(d2);
			for (int b2 = (d1 == d2 ? e1 + 1 : 0); b2 < len2; b2++)
			if (IS_REQUEST(solution, d2, b2))
			{
				if (consider_reschedule(solution, d1, b1, e1, d2, b2, obj) < 0)
				{
					if (!cont) return true;
					retval = true;
					len1 = solution->get_number_of_stops(d1);
					len2 = solution->get_number_of_stops(d2);
					if (!IS_REQUEST(solution, d1, b1)) { b2 = len2; d2 = num_drivers; e1 = len1; b1 = len1; continue; }
					if (!IS_REQUEST(solution, d1, e1)) { b2 = len2; d2 = num_drivers; e1 = len1; continue; }
					if (!IS_REQUEST(solution, d2, b2)) { b2 = len2; continue; }
				}
				for (int e2 = b2 + 1; e2 < len2; e2++)
				if (IS_REQUEST(solution, d2, e2))
				if (consider_exchange(solution, d1, b1, e1, d2, b2, e2, obj) < 0)
				{
					if (!cont) return true;
					retval = true;
					len1 = solution->get_number_of_stops(d1);
					len2 = solution->get_number_of_stops(d2);
					if (!IS_REQUEST(solution, d1, b1)) { e2 = len2; b2 = len2; d2 = num_drivers; e1 = len1; b1 = len1; continue; }
					if (!IS_REQUEST(solution, d1, e1)) { e2 = len2; b2 = len2; d2 = num_drivers; e1 = len1; continue; }
					if (!IS_REQUEST(solution, d2, b2)) { e2 = len2; b2 = len2; continue; }
				}
			}
		}
	}

#if 0

	// what does this do?

	for (int d1 = 0; d1 < num_drivers; d1++)
	{
		int len1 = solution->get_number_of_stops(d1);
		for (int b1 = 0; b1 < len1; b1++)
		if (IS_REQUEST(solution, d1, b1))
		for (int d2 = d1; d2 < num_drivers; d2++)
		{
			int len2 = solution->get_number_of_stops(d2);
			for (int b2 = (d1 == d2 ? b1 + 1 : 0); b2 < len2; b2++)
			if (IS_REQUEST(solution, d2, b2))
			{
				if (consider_reschedule(solution, d1, b1, b1, d2, b2, obj) < 0)
				{
					if (!cont) return true;
					len1 = solution->get_number_of_stops(d1);
					len2 = solution->get_number_of_stops(d2);
					if (!IS_REQUEST(solution, d1, b1)) { b2 = len2; d2 = num_drivers; b1 = len1; continue; }
					if (!IS_REQUEST(solution, d2, b2)) { b2 = len2; continue; }
				}
			}
		}
	}
#endif

	return retval;
}

void request_exchange_search(Solution* solution)
{
	solution_exchange_iterator iterator {solution};

	int pb2 = -1;

	while (iterator.is_valid())
	{
		if (pb2 != iterator.sub2.begin)
		{
			if (apply_reschedule(solution,
					iterator.sub1.driver, iterator.sub1.begin, iterator.sub1.end,
					iterator.sub2.driver, iterator.sub2.begin))
			{
				iterator.solution_changed();
				continue;
			}
		}

		if (apply_exchange(solution,
				iterator.sub1.driver, iterator.sub1.begin, iterator.sub1.end,
				iterator.sub2.driver, iterator.sub2.begin, iterator.sub2.end))
		{
			iterator.solution_changed();
			continue;
		}

		iterator.increment();
		if (iterator.at_last_change())
		{
			break;
		}
	}
}