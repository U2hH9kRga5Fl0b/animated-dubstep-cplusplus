/*
 * neighbor.cpp
 *
 *  Created on: Nov 8, 2014
 *      Author: thallock
 */

#include "opts/neighbor.h"


#include "opts/find_path.h"





#define TEST_IMPROVEMENT_MEASURE 1







namespace
{

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
			driver,
			solution->get_action_index(driver, i  ),
			solution->get_action_index(driver, i+1));
	}
	return sum;
}

int *get_all_times(const Solution* solution, int&len)
{
	len = solution->get_num_drivers();
	int * retval = new int[len];
	for (int i = 0; i < len; i++)
		retval[i] = solution->get_time_for_driver(i);
	return retval;
}

int get_overtime_cost(int *times, int len, int overtime_cost, int& minindex)
{
	int mintime = INT_MAX;

	for (int i = 0; i < len; i++)
		if (times[i] < mintime)
		{
			mintime = times[i];
			minindex = i;
		}

	int retval = 0;
	for (int i = 0; i < len; i++)
		retval += (times[i] - mintime) * overtime_cost;
	return retval;
}

int get_overtime(const Solution* solution, int overtime_cost)
{
	int len;
	int minindex;
	int *ts = get_all_times(solution, len);
	int ret = get_overtime_cost(ts, len, overtime_cost, minindex);
	delete[] ts;
	return ret;
}


}


int consider_exchange(Solution* s, int d1, int b1, int e1, int d2, int b2, int e2, const objective *obj, bool apply)
{
	if (d1 == d2)
	{
		// not implemented yet...
		return INT_MAX;
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

	// Never completely remove a driver's path
	if (pa1 == BEGIN_INDEX && na1 == END_INDEX)
		return INT_MAX;
	if (pa2 == BEGIN_INDEX && na2 == END_INDEX)
		return INT_MAX;

	int p1[MAX_PATH]; int p1len; int p1time;
	int p2[MAX_PATH]; int p2len; int p2time;
	int p3[MAX_PATH]; int p3len; int p3time;
	int p4[MAX_PATH]; int p4len; int p4time;

	find_path_between_requests(s, d1, pa1, fa2, &p1[0], p1len, p1time);
	find_path_between_requests(s, d1, la2, na1, &p2[0], p2len, p2time);
	find_path_between_requests(s, d2, pa2, fa1, &p3[0], p3len, p3time);
	find_path_between_requests(s, d2, la1, na2, &p4[0], p4len, p4time);

	int old_time_cost = TEST_IMPROVEMENT_MEASURE ? s->sum_all_times() : 0;
	int old_overtime_cost = TEST_IMPROVEMENT_MEASURE ? get_overtime(s, obj->overtime_cost) : 0;


	int change_in_time = INT_MAX;
	int change_in_overtime = INT_MAX;

	int change_in_objective = INT_MAX;

	switch(obj->type)
	{
		case relink:
		{
			trap();
		}
		break;
		case times_sum:
		{
			int firstlen  = plength(s, d1, b1, e1);
			int secondlen = plength(s, d2, b2, e2);

			int nt1 = p1time + p2time + secondlen;
			int nt2 = p3time + p4time + firstlen;
			int nt = nt1 + nt2;

			int ot1 = plength(s, d1, p1i, b1) + firstlen  + plength(s, d1, e1, n1i);
			int ot2 = plength(s, d2, p2i, b2) + secondlen + plength(s, d2, e2, n2i);
			int ot = ot1 + ot2;

			change_in_time = nt - ot;
			if (obj->overtime_cost == 0)
			{
				change_in_objective = change_in_time;
			}

			int len, minindex;
			int *ts = get_all_times(s, len);
			int oldovertime = get_overtime_cost(ts, len, obj->overtime_cost, minindex);

			ts[d1] += nt1 - ot1;
			ts[d2] += nt2 - ot2;

			int newovertime = get_overtime_cost(ts, len, obj->overtime_cost, minindex);

			change_in_overtime = newovertime - oldovertime;
			change_in_objective = change_in_time + change_in_overtime;
		}
		break;
		default:
			trap();
	}

	if (!apply || change_in_objective >= 0)
		return change_in_objective;


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

	if (TEST_IMPROVEMENT_MEASURE)
	{
		if (obj->type == times_sum)
		{
			int ntime = s->sum_all_times();
			if (ntime != old_time_cost + change_in_time)
			{
				trap();
			}

			int novertime = get_overtime(s, obj->overtime_cost);
			if (old_overtime_cost + change_in_overtime != novertime)
			{
				trap();
			}
		}
		else
		{
			trap();
		}
	}

	return change_in_objective;
}


int consider_reschedule(Solution* s, int d1, int b1, int e1, int d2, int b2,
		const objective *obj, bool apply)
{
	if (d1 == d2)
	{
		// not implemented yet...
		return INT_MAX;
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

	// Never completely remove a driver's path
	if (pa1 == BEGIN_INDEX && na1 == END_INDEX)
		return INT_MAX;
	if (pa2 == BEGIN_INDEX && na2 == END_INDEX)
		return INT_MAX;

	int p1[MAX_PATH]; int p1len; int p1time;
	int p2[MAX_PATH]; int p2len; int p2time;
	int p3[MAX_PATH]; int p3len; int p3time;

	find_path_between_requests(s, d2, pa2, fa1, &p1[0], p1len, p1time);
	find_path_between_requests(s, d2, la1, na2, &p2[0], p2len, p2time);
	find_path_between_requests(s, d1, pa1, na1, &p3[0], p3len, p3time);


	int old_time_cost = TEST_IMPROVEMENT_MEASURE ? s->sum_all_times() : 0;
	int old_overtime_cost = TEST_IMPROVEMENT_MEASURE ? get_overtime(s, obj->overtime_cost) : 0;

	int change_in_time = INT_MAX;
	int change_in_overtime = INT_MAX;
	int change_in_objective = INT_MAX;

	switch(obj->type)
	{
		case relink:
		{
			trap();
		}
		break;
		case max_time:
		{
#if 0
			int cutlen = plength(s, d1, b1, e1);

			int nt1 = p3time;
			int nt2 = p1time + p2time + cutlen;
			int nt = nt1 + nt2;

			int ot1 = plength(s, d1, p1i, b1) + cutlen + plength(s, d1, e1, n1i);
			int ot2 = plength(s, d2, p2i, n2i);
			int ot = ot1 + ot2;

			int len, minindex;
			int *ts = get_all_times(s, len);
			if (minindex != d1 && minindex != d2)
			{
				delete ts;
				return INT_MAX;
			}
#endif
		}
		break;
		case times_sum:
		{
			int cutlen = plength(s, d1, b1, e1);

			int nt1 = p3time;
			int nt2 = p1time + p2time + cutlen;
			int nt = nt1 + nt2;

			int ot1 = plength(s, d1, p1i, b1) + cutlen + plength(s, d1, e1, n1i);
			int ot2 = plength(s, d2, p2i, n2i);
			int ot = ot1 + ot2;

			change_in_time = nt - ot;
			if (obj->overtime_cost == 0)
			{
				change_in_objective = change_in_time;
			}

			int len, minindex;
			int *ts = get_all_times(s, len);
			int oldovertime = get_overtime_cost(ts, len, obj->overtime_cost, minindex);
			ts[d1] += nt1 - ot1;
			ts[d2] += nt2 - ot2;
			int newovertime = get_overtime_cost(ts, len, obj->overtime_cost, minindex);

			change_in_overtime = newovertime - oldovertime;
			change_in_objective = change_in_time + change_in_overtime;
		}
		break;
		default:
			trap();
	}

	if (!apply || change_in_objective >= 0)
		return change_in_objective;

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


	if (TEST_IMPROVEMENT_MEASURE)
	{
		if (obj->type == times_sum)
		{
			int ntime = s->sum_all_times();
			if (ntime != old_time_cost + change_in_time)
			{
				trap();
			}

			int novertime = get_overtime(s, obj->overtime_cost);
			if (old_overtime_cost + change_in_overtime != novertime)
			{
				trap();
			}
		}
		else
		{
			trap();
		}
	}

	return change_in_objective;
}
