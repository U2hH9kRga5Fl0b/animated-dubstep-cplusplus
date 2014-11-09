/*
 * requests_swap.cpp
 *
 *  Created on: Nov 2, 2014
 *      Author: thallock
 */



/*
 * codon.cpp
 *
 *  Created on: Oct 25, 2014
 *      Author: thallock
 */

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


#define IS_REQUEST(solution, d, s)       (solution->get_city()->get_action(solution->get_action_index(d, s)).value)

int fpr(const Solution* solution, int driver, int index)
{
	for (int i = index - 1; i >= 0; i--)
		if (IS_REQUEST(solution, driver, i))
			return i;
	return -1;
}
int fnr(const Solution* solution, int driver, int index)
{
	const int length = solution->get_number_of_stops(driver);
	for (int i = index + 1; i < length; i++)
		if (IS_REQUEST(solution, driver, i))
			return i;
	return length;
}


int plength(const Solution* solution, int driver, int start, int end)
{
	int sum = 0;
	for (int i = start; i < end; i++)
	{
		sum += solution->get_city()->get_time_to(
			solution->get_action_index(driver, i  ),
			solution->get_action_index(driver, i+1));
	}
	return sum;
}

bool apply_exchange(Solution* s, int d1, int b1, int e1, int d2, int b2, int e2)
{
	if (d1 == d2)
	{
		// not implemented yet...
		return false;
	}

	int p1i = fpr(s, d1, b1);
	int n1i = fnr(s, d1, e1);
	int p2i = fpr(s, d2, b2);
	int n2i = fnr(s, d2, e2);

	int pa1 = s->get_action_index(d1, p1i);
	int fa1 = s->get_action_index(d1, b1);
	int la1 = s->get_action_index(d1, e1);
	int na1 = s->get_action_index(d1, n1i);

	int pa2 = s->get_action_index(d2, p2i);
	int fa2 = s->get_action_index(d2, b2);
	int la2 = s->get_action_index(d2, e2);
	int na2 = s->get_action_index(d2, n2i);

	int p1[MAX_PATH]; int p1len; int p1time;
	int p2[MAX_PATH]; int p2len; int p2time;
	int p3[MAX_PATH]; int p3len; int p3time;
	int p4[MAX_PATH]; int p4len; int p4time;

	find_path_between_requests(s, pa1, fa2, &p1[0], p1len, p1time);
	find_path_between_requests(s, la2, na1, &p2[0], p2len, p2time);
	find_path_between_requests(s, pa2, fa1, &p3[0], p3len, p3time);
	find_path_between_requests(s, la1, na2, &p4[0], p4len, p4time);

	int nt = p1time + p2time + p3time + p4time;
	int ot = plength(s, d1, p1i, b1) + plength(s, d1, e1, n1i) + plength(s, d2, p2i, b2) + plength(s, d2, e2, n2i);


	int old_cost = s->sum_all_times();

	int time_delta = nt - ot;
	if (time_delta >= 0)
		return false;

	log() << "APPLY NEIGHBOR OPERATION: move subpath from [driver:" << std::setw(3) << d1 << ",begin=" << std::setw(3) << b1 << ",end=" << std::setw(3) << e1 << "] to after [driver:" << std::setw(3) << d2 << ",begin=" << std::setw(3) << b2 << ",end=" << std::setw(3) << e2 << "]\n";
//	std::cout << d1 << ", " << b1 << ", " << e1 << ", " << d2 << ", " << b2 << ", " << e2 << std::endl;
//	std::cout << pa1 << ", " << fa1 << ", " << la1 << ", " << na1 << std::endl;
//	std::cout << pa2 << ", " << fa2 << ", " << la2 << ", " << na2 << std::endl;

	// Do the dirty work...

	int olen1 = n1i - p1i;
	int olen2 = n2i - p2i;

	int nlen1 = e2 - b2 + p1len + p2len;
	int nlen2 = e1 - b1 + p3len + p4len;

	int t1len = e1 - b1 + 1;
	int *p1tmps = new int[t1len];
	for (int i = 0; i < t1len; i++)
		p1tmps[i] = s->get_action_index(d1, b1 + i);
	int t2len = e2 - b2 + 1;
	int *p2tmps = new int[t2len];
	for (int i = 0; i < t2len; i++)
		p2tmps[i] = s->get_action_index(d2, b2 + i);

	s->shift(d1, n1i, nlen1 - olen1 + 2);
	s->shift(d2, n2i, nlen2 - olen2 + 2);

	int ndx = p1i + 1;
	for (int i = 0; i < p1len; i++)
		s->set_action(d1, ndx++, p1[i]);
	for (int i = 0; i < t2len; i++)
		s->set_action(d1, ndx++, p2tmps[i]);
	for (int i = 0; i < p2len; i++)
		s->set_action(d1, ndx++, p2[i]);

	ndx = p2i + 1;
	for (int i = 0; i < p3len; i++)
		s->set_action(d2, ndx++, p3[i]);
	for (int i = 0; i < t1len; i++)
		s->set_action(d2, ndx++, p1tmps[i]);
	for (int i = 0; i < p4len; i++)
		s->set_action(d2, ndx++, p4[i]);

	delete[] p1tmps;
	delete[] p2tmps;
	s->refresh();

	int ntime = s->sum_all_times();
	if (ntime != old_cost + time_delta)
	{
		trap();
	}

	return true;
}


bool apply_reschedule(Solution* s, int d1, int b1, int e1, int d2, int b2)
{
	if (d1 == d2)
	{
		// not implemented yet...
		return false;
	}

	int p1i = fpr(s, d1, b1);
	int n1i = fnr(s, d1, e1);
	int p2i = fpr(s, d2, b2);
	int n2i = b2;

	int pa1 = s->get_action_index(d1, p1i);
	int fa1 = s->get_action_index(d1, b1);
	int la1 = s->get_action_index(d1, e1);
	int na1 = s->get_action_index(d1, n1i);

	int pa2 = s->get_action_index(d2, p2i);
	int na2 = s->get_action_index(d2, n2i);

	int p1[MAX_PATH]; int p1len; int p1time;
	int p2[MAX_PATH]; int p2len; int p2time;
	int p3[MAX_PATH]; int p3len; int p3time;

	find_path_between_requests(s, pa2, fa1, &p1[0], p1len, p1time);
	find_path_between_requests(s, la1, na2, &p2[0], p2len, p2time);
	find_path_between_requests(s, pa1, na1, &p3[0], p3len, p3time);

	int nt = p1time + p2time + p3time;
	int ot = plength(s, d1, p1i, b1) + plength(s, d1, e1, n1i) + plength(s, d2, p2i, n2i);

	int old_cost = s->sum_all_times();

	int time_delta = nt - ot;
	if (time_delta >= 0)
		return false;

	log() << "APPLY NEIGHBOR OPERATION: move subpath from [driver:" << std::setw(3) << d1 << ",begin=" << std::setw(3) << b1 << ",end=" << std::setw(3) << e1 << "] to after [driver:" << std::setw(3) << d2 << ",begin=" << std::setw(3) << b2 << "]\n";
//	std::cout << d1 << ", " << b1 << ", " << e1 << ", " << d2 << ", " << b2 << std::endl;
//	std::cout << pa1 << ", " << fa1 << ", " << la1 << ", " << na1 << std::endl;

	int olen1 = n1i - p1i;
	int olen2 = n2i - p2i;

	int nlen1 = p3len;
	int nlen2 = p1len + e1-b1+1 + p2len;

	int t1len = e1 - b1 + 1;
	int *p1tmps = new int[t1len];
	for (int i = 0; i < t1len; i++)
		p1tmps[i] = s->get_action_index(d1, b1 + i);

	s->shift(d1, n1i, nlen1 - olen1 + 1);
	s->shift(d2, n2i, nlen2 - olen2 + 1);

	int ndx = p1i + 1;
	for (int i = 0; i < p3len; i++)
		s->set_action(d1, ndx++, p3[i]);

	ndx = p2i + 1;
	for (int i = 0; i < p1len; i++)
		s->set_action(d2, ndx++, p1[i]);
	for (int i = 0; i < t1len; i++)
		s->set_action(d2, ndx++, p1tmps[i]);
	for (int i = 0; i < p2len; i++)
		s->set_action(d2, ndx++, p2[i]);

	delete[] p1tmps;
	s->refresh();

	int ntime = s->sum_all_times();
	if (ntime != old_cost + time_delta)
	{
		trap();
	}

	return true;
}

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


bool apply_random_request_exchange(Solution* solution, int num_attempts)
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
		if (apply_reschedule(solution, driver1, begin1, end1, driver2, begin2))
		{
			return true;
		}
		if (apply_exchange(solution, driver1, begin1, end1, driver2, begin2, end2))
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




bool apply_first_exchange(Solution* solution)
{
	const int num_drivers = solution->get_num_drivers();
	for (int d1 = 0; d1 < num_drivers; d1++)
	{
		const int len1 = solution->get_number_of_stops(d1);
		for (int b1 = 0; b1 < len1; b1++)
		if (IS_REQUEST(solution, d1, b1))
		for (int e1 = b1 + 1; e1 < len1; e1++)
		if (IS_REQUEST(solution, d1, e1))
		for (int d2 = d1; d2 < num_drivers; d2++)
		{
			const int len2 = solution->get_number_of_stops(d2);
			for (int b2 = (d1 == d2 ? e1 + 1 : 0); b2 < len2; b2++)
			if (IS_REQUEST(solution, d2, b2))
			{
				if (apply_reschedule(solution, d1, b1, e1, d2, b2))
					return true;
				for (int e2 = b2 + 1; e2 < len2; e2++)
				if (IS_REQUEST(solution, d2, e2))
				if (apply_exchange(solution, d1, b1, e1, d2, b2, e2))
					return true;
			}
		}
	}

	return false;
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
