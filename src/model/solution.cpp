/*
 * solution.cpp
 *
 *  Created on: Oct 23, 2014
 *      Author: thallock
 */

#include "solution.h"

#include "common.h"

Solution::Solution(const City* c_, int starting_length) :
	c{c_},
	stops{c->num_trucks, starting_length},
	drivers{new int[c->num_actions]},
	stop_numbers{new int[c->num_actions]},
	lens{new int[c->num_trucks]},
	times{c->num_trucks, starting_length}
{
	stops = -1;

	for (int i = 0; i < c->num_trucks; i++)
	{
		lens[i] = 0;
	}
	for (int i = 0; i < c->num_actions; i++)
	{
		drivers[i] = stop_numbers[i] = -1;
	}

	times = 0;
	ensure_valid();
}



Solution::~Solution()
{
	delete[] drivers;
	delete[] stop_numbers;
	delete[] lens;
}

void Solution::service(int driver, int stop, int action)
{
	if (action < 0)
	{
		std::cerr << "this doesn't work yet!" << std::endl;
		trap();
	}
	if (stop >= stops.cols())
	{
		int numnewcols = std::max(stop, (int) (1.5 * stops.cols()));
		stops.set_num_columns(numnewcols);
		times.set_num_columns(numnewcols);

	}
	INBOUNDS(0, driver, get_num_drivers());
	INBOUNDS(0, stop, stops.cols());

	stops.at(driver, stop) = action;
	drivers[action] = driver;
	stop_numbers[action] = stop;
	lens[driver] = std::max(lens[driver], stop + 1);

	times.at(driver, stop) = stop != 0 ?
			times.at(driver, stop-1) + c->get_time_to(stops.at(driver, stop - 1), action) :
			c->get_start_time(action);

	for (int i = stop + 1; i < lens[driver]; i++)
	{
		times.at(driver, i) = times.at(driver, i-1) + c->get_time_to(stops.at(driver, i-1), stops.at(driver, i));
	}

	ensure_valid();
}

int Solution::get_action(int driver, int stop) const
{
	INBOUNDS(0, driver, get_num_drivers());
	INBOUNDS(0, stop, stops.cols());
	return stops.at(driver, stop);
}

int Solution::get_num_serviced() const
{
	int ret_val = 0;

	int num_drivers = get_num_drivers();
	for (int d = 0; d < num_drivers; d++)
	{
		int len = lens[d];

		for (int s = 1; s < len; s++)
		{
			ret_val += c->get_action(stops.at(d, s)).value;
		}
	}
	return ret_val;
}

int Solution::get_time_for(int driver) const
{
	INBOUNDS(0, driver, stops.rows());
	if (lens[driver] <= 0)
	{
		return 0;
	}
	int la = stops.at(driver, lens[driver]-1);
	return times.at(driver, lens[driver]-1) + c->durations.at(c->get_action(la).location, c->start_location);
}

int Solution::get_time() const
{
	int sum = 0;

	int num_drivers = get_num_drivers();
	for (int i = 0; i < num_drivers; i++)
	{
		sum += get_time_for(i);
	}

	return sum;
}

bool Solution::already_serviced(int action) const
{
	INBOUNDS(0, action, c->num_actions);
	if (!c->get_action(action).value)
	{
		return false;
	}
	return drivers[action] >= 0;
}

void Solution::ensure_valid() const
{
	if (!DEBUG)
	{
		return;
	}

	if (times.rows() != stops.rows() || times.cols() != stops.cols())
	{
		std::cerr << "this is bad" << std::endl;
		trap();
	}

	int num_drivers = get_num_drivers();
	int rlen = stops.cols();

	int *counts = new int[rlen];
	for (int i = 0; i < rlen; i++)
	{
		counts[i] = 0;
	}

	for (int d = 0; d < num_drivers; d++)
	{
		int len = lens[d];
		if (len < 0 || len >= rlen)
		{
			std::cerr << (*this) << std::endl;
			std::cerr << "there is a bad length for driver " << d << std::endl;
			trap();
		}

		for (int s = 0; s < rlen; s++)
		{
			int stop = stops.at(d, s);
			if (s < len)
			{
				if (stop < 0)
				{
					std::cerr << (*this) << std::endl;
					std::cerr << "there should not be a negative this early in the solution." << std::endl;
					trap();
				}

				if (s == 0 && !c->is_start_action(stop))
				{
					std::cerr << (*this) << std::endl;
					std::cerr << "driver " << d << " starts with a dumpster." << std::endl;
					trap();
				}

				if (c->get_action(stop).value)
				{
					if (counts[stop]++ > 1)
					{
						std::cerr << (*this) << std::endl;
						std::cerr << "visited stop " << stop << " multiple times." << std::endl;
						trap();
					}

					if (stop_numbers[stop] != s)
					{
						std::cerr << (*this) << std::endl;
						std::cerr << "the indices for stop " << stop << " at " << d << "," << s << " were wrong!" << std::endl;
						trap();
					}

					if (drivers[stop] != d)
					{
						std::cerr << (*this) << std::endl;
						std::cerr << "the driver number for stop " << stop << " were wrong!" << std::endl;
						trap();
					}
				}
			}
			else
			{
				if (stop >= 0)
				{
					std::cerr << (*this) << std::endl;
					std::cerr << "there should not be a stop this late in the solution." << std::endl;
					trap();
				}
			}
		}
	}

	for (int i = 0; i < c->num_actions; i++)
	{
		int stop_number = stop_numbers[i];
		int driver = drivers[i];
		if (stop_number < 0)
		{
			if (driver >= 0)
			{
				std::cerr << (*this) << std::endl;
				std::cerr << "there is a stop number for action " << i << " but no driver" << std::endl;
				trap();
			}
			continue;
		}

		if (stops.at(driver, stop_number) != i)
		{
			std::cerr << (*this) << std::endl;
			std::cerr << "the stop for driver " << driver << " index " << stop_number << " is supposed to be " << i << std::endl;
			trap();
		}
	}

	delete counts;

	// verify the times...
	for (int d = 0; d < num_drivers; d++)
	{
		int len = lens[d];
		if (len == 0)
		{
			continue;
		}

		if (times.at(d, 0) != c->get_start_time(stops.at(d, 0)))
		{
			std::cerr << (*this) << std::endl;
			std::cout << "the first time for driver " << d << " is bad." << std::endl;
			trap();
		}

		for (int s = 1; s < len; s++)
		{
			if (times.at(d, s) != times.at(d, s-1) + c->get_time_to(stops.at(d, s-1), stops.at(d, s)))
			{
				std::cerr << (*this) << std::endl;
				std::cout << "the " << s << "-th time for driver " << d << " is bad." << std::endl;
				trap();
			}
		}
	}
}

std::ostream& operator<<(std::ostream& out, const Solution& sol)
{
	if (DEBUG)
	{
		for (int d = 0; d < sol.get_num_drivers(); d++)
		{
			out << std::setw(2) << d << " len=" << std::setw(3) << sol.lens[d] << ':';
			for (int s = 0; s < sol.stops.cols(); s++)
			{
				out << std::setw(3) << sol.get_action(d, s) << ' ';
			}
			out << std::endl;
		}

		out << "indices: ";
		for (int i=0;i<sol.c->num_actions;i++)
		{
			out << std::setw(3) << i << " ";
		}
		out << std::endl;
		out << "drivers: ";
		for (int i=0;i<sol.c->num_actions;i++)
		{
			out << std::setw(3) << sol.drivers[i] << " ";
		}
		out << std::endl;
		out << "stop nu: ";
		for (int i=0;i<sol.c->num_actions;i++)
		{
			out << std::setw(3) << sol.stop_numbers[i] << " ";
		}
		out << std::endl;

		out << "times:" << std::endl;
		out << sol.times << std::endl;
	}
	else
	{
		for (int d = 0; d < sol.get_num_drivers(); d++)
		{
			int len = sol.get_length(d);
			for (int s = 0; s < len; s++)
			{
				out << sol.get_action(d, s) << ' ';
			}
			out << "| ";
		}
		out << std::endl;
	}

	return out;
}

Coord Solution::get_location_at(int driver, int time, int *action) const
{
	INBOUNDS(0, time, INT_MAX);
	int prevloc, nextloc, ftime, mtime, ltime, pac, nac;

	if (lens[driver] <= 0)
	{
		return c->coords[c->start_location];
	}

	if (time < times.at(driver, 0))
	{
		ftime = 0;
		mtime = c->durations.at(c->start_location, c->get_action(stops.at(driver, 0)).location);
		ltime = mtime + c->get_action(stops.at(driver, 0)).wait_time;
		pac = -1;
		nac = stops.at(driver, 0);

		prevloc = c->start_location;
		nextloc = c->get_action(stops.at(driver, 0)).location;
	}
	else if (time >= times.at(driver, lens[driver]-1))
	{
		ftime = times.at(driver, lens[driver]-1);
		mtime = get_time_for(driver);
		ltime = INT_MAX-1;

		prevloc = c->get_action(stops.at(driver, lens[driver] - 1)).location;
		nextloc = c->start_location;

		pac = stops.at(driver, lens[driver]-1);
		nac = -1;
	}
	else
	{
		int validIndex = times.binary_search(driver, time, lens[driver]-1);
		ftime = times.at(driver, validIndex  );
		ltime = times.at(driver, validIndex+1);
		mtime = ltime - c->get_action(stops.at(driver, validIndex+1)).wait_time;
		pac = stops.at(driver, validIndex  );
		nac = stops.at(driver, validIndex+1);
		prevloc = c->get_action(pac).location;
		nextloc = c->get_action(nac).location;

	}

	INBOUNDS(ftime, mtime, ltime);

	if (time > mtime || prevloc == nextloc)
	{
		if (action != nullptr)
		{
			(*action) = nac;
		}
		return c->coords[nextloc];
	}

	if (action != nullptr)
	{
		(*action) = pac;
	}

	if (mtime == ftime)
	{
		std::cerr << "oh dear." << std::endl;
		trap();
	}

	double percent = (time - ftime) / (double) (mtime - ftime);
	return Coord {c->coords[prevloc].x + percent * (c->coords[nextloc].x - c->coords[prevloc].x),
		      c->coords[prevloc].y + percent * (c->coords[nextloc].y - c->coords[prevloc].y)};
}

void Solution::human_readable(std::ostream& out) const
{
	std::cout << "total time: " << get_time() << std::endl;
	for (int d = 0; d < get_num_drivers(); d++)
	{
		out << "driver: " << (d+1) << std::endl;
		int len = get_length(d);
		out << "\tlength: " << len << std::endl;

		int prevloc = c->start_location;

		int t=0;
		for (int s = 0; s < len; s++)
		{
			int nloc = c->get_action(get_action(d, s)).location;
			t += c->durations.at(prevloc, nloc);
			out << "\t\t[stop " << std::setw(2) << (s+1) << "][t=" << std::setw(5) << t << "] stop=" << c->get_action(get_action(d, s)) << std::endl;
			t += c->get_action(get_action(d, s)).wait_time;
			prevloc = nloc;
		}
		out << std::endl;
		out << "\tend time=" << t << std::endl;
	}
}

int Solution::get_last_time() const
{
	int max = INT_MIN;

	int num_drivers = get_num_drivers();
	for (int i=0;i<num_drivers;i++)
	{
		max = std::max(max, get_time_for(i));
	}

	return max;
}
