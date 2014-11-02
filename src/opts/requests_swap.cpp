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
		if (IS_REQUEST(solution, driver, index))
			return i;
	return -1;
}
int fnr(const Solution* solution, int driver, int index)
{
	const int length = solution->get_number_of_stops(driver);
	for (int i = index + 1; i < length; i++)
		if (IS_REQUEST(solution, driver, index))
			return i;
	return length;
}


int plength(const Solution* solution, int driver, int start, int end)
{
	int sum = 0;

	// should probably start at start-1
	for (int i = std::max(0, start - 1); i < end; i++)
	{
		sum += solution->get_city()->get_time_to(i, i + 1);
	}

	return sum;
}



#define MAX_PATH 5
void find_path(const Solution* solution, int a1, int a2, int *path, int& len, int& time)
{
	const City* c = solution->get_city();
	// P->P
	// P->D
	// P->R
	// D->P
	// D->D
	// D->R
	// R->P
	// R->D
	// R->R

	time = INT_MAX;

	int tmp_path;
	int tmp_len;

	// if D->P
	// return time

	if (D->P)
	{
		tmp_len = 0;
		time = c->get_time_to(a1, a2);
	}
	else if ()
	{
		for (int l = 0; l < c->num_landfills; l++)
		{
			const int t1 = c->get_time_to(a1, l);
			for (int s = 0; s < c->num_stagingareas; s++)
			{
				int tmp_time = t1 + c->get_time_to(l, s) + c->get_time_to(s, a2);
				if (tmp_time >= time)
				{
					continue;
				}
				time = tmp_time;
				path[0] = l; path[1] = s;
				tmp_len = 2;
			}
		}
	}
	else
	{
		for (int s = 0; s < c->num_stagingareas; s++)
		{
			int tmp_time = c->get_time_to(a1, s) + c->get_time_to(s, a2);
			if (tmp_time >= time)
			{
				continue;
			}
			time = tmp_time;
			path[0] = s;
			tmp_len = 1;
		}
	}
}

bool apply_exchange(Solution* s, int d1, int b1, int e1, int d2, int b2, int e2)
{
	int pa1 = s->get_action_index(d1, fpr(s, d1, b1));
	int fa1 = s->get_action_index(d1, b1);
	int la1 = s->get_action_index(d1, e1);
	int na1 = s->get_action_index(d1, fnr(s, d1, b1));

	int pa2 = s->get_action_index(d2, fpr(s, d2, b2));
	int fa2 = s->get_action_index(d2, b2);
	int la2 = s->get_action_index(d2, e2);
	int na2 = s->get_action_index(d2, fnr(s, d2, b2));

	int p1[MAX_PATH]; int p1len; int p1time;
	int p2[MAX_PATH]; int p2len; int p2time;
	int p3[MAX_PATH]; int p3len; int p3time;
	int p4[MAX_PATH]; int p4len; int p4time;

	find_path(s, pa1, fa2, &p1[0], p1len, p1time);
	find_path(s, la2, na1, &p2[0], p2len, p2time);
	find_path(s, pa2, fa1, &p3[0], p3len, p3time);
	find_path(s, la1, na2, &p4[0], p4len, p4time);

	int nt = p1time + p2time + p3time + p4time;
	int ot = plength(s, d1, pa1, fa1) + plength(s, d1, la1, na1) + plength(s, d2, pa2, fa2) + plength(s, d1, la2, na2);


	int time_delta = nt - ot;
	if (time_delta < 0)
	{
		return false;
	}

	// do some dirty work...

	return true;
}

int get_random_request(const Solution* solution, int driver, int length, int avoid)
{
	int ndx = rand() % length;

	for (int i = 0; i < length; i++, ndx++)
	{
		if (ndx > length) ndx = 0;
		if (IS_REQUEST(solution, driver, ndx) && ndx != avoid)
			return i;
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
			for (int e2 = b2 + 1; e2 < len2; e2++)
			if (IS_REQUEST(solution, d2, e2))
			if (apply_exchange(solution, d1, b1, e1, d2, b2, e2))
				return true;
		}
	}

	return false;
}










































#if 0
bool apply_first_random_exchange(Solution* solution)
{
	int num_drivers = solution->get_num_drivers();
	for (int d1 = 0; d1 < num_drivers; d1++)
	{
		int len1 = solution->get_number_of_stops(d1);
		for (int b1 = 0; b1 < len1; b1++)
		{
			if (!IS_REQUEST(solution, d1, b1))
			{
				continue;
			}
			for (int e1 = b1 + 1; e1 < len1; e1++)
			{
				if (!IS_REQUEST(solution, d1, e1))
				{
					continue;
				}

				for (int d2 = d1; d2 < num_drivers; d2++)
				{
					int len2 = solution->get_number_of_stops(d2);
					for (int b2 = (d1 == d2 ? e1 + 1 : 0); b2 < len2; b2++)
					{
						if (!IS_REQUEST(solution, d2, b2))
						{
							continue;
						}
						for (int e2 = b2 + 1; e2 < len2; e2++)
						{
							if (!IS_REQUEST(solution, d2, e2))
							{
								continue;
							}
							if (apply_exchange(solution, d1, b1, e1, d2, b2, e2))
							{
								return true;
							}
						}
					}
				}
			}
		}
	}

	return false;
}
#endif

