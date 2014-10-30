/*
 * cost.h
 *
 *  Created on: Oct 29, 2014
 *      Author: thallock
 */

#ifndef COST_H_
#define COST_H_

class cost
{
public:
	int num_serviced;
	int max_time;
	int total_time;

	cost() : num_serviced{0}, max_time{0}, total_time{0} {}
	cost(Solution* sol) : num_serviced{sol->get_num_serviced()}, max_time{sol->get_maximum_time()}, total_time{sol->sum_all_times()} {}
	cost(int n, int m, int s) : num_serviced{n}, max_time{m}, total_time{s} {}
	~cost();

	bool operator==(const cost& other) const
	{
		return num_serviced == other.num_serviced
			&& max_time == other.max_time
			&& total_time == other.total_time;
	}
	bool operator<(const cost& other) const
	{
		if (num_serviced != other.num_serviced)
		{
			return num_serviced < other.num_serviced;
		}
		if (total_time != other.total_time)
		{
			return total_time < other.total_time;
		}
		if (max_time != other.max_time)
		{
			return max_time < other.max_time;
		}
		return false;
	}
};

#endif /* COST_H_ */
