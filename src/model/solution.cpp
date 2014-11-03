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
	times{c->num_trucks, starting_length},
	invs{c_}
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

Solution::Solution(const Solution& other) :
		Solution(other.c, other.stops.cols())
{
	(*this) = other;
}
Solution& Solution::operator =(const Solution& other)
{
	if (other.c != c)
	{
		err() << "can't make this assignment." << std::endl;
		trap();
	}

	for (int d = 0; d < other.stops.rows(); d++)
	{
		for (int s = 0; s < other.lens[d]; s++)
		{
			stops.at(d, s) = other.stops.at(d, s);
		}
	}

	refresh();
	return (*this);
}

Solution::~Solution()
{
	delete[] drivers;
	delete[] stop_numbers;
	delete[] lens;
}

int Solution::get_action_index(int driver, int stop) const
{
	INBOUNDS(0, driver, get_num_drivers());
	INBOUNDS(-1, stop, stops.cols()+1);
	if (stop == -1)
	{
		return START_ACTION_INDEX;
	}
	else if (stop == lens[driver])
	{
		return END_ACTION_INDEX;
	}
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

int Solution::get_time_for_driver(int driver) const
{
	INBOUNDS(0, driver, stops.rows());
	if (lens[driver] <= 0)
	{
		return 0;
	}
	int la = stops.at(driver, lens[driver]-1);
	int lat = times.at(driver, lens[driver]-1);
	int time_to_fin = c->durations.at(c->get_action(la).location, c->start_location);
	return lat + time_to_fin;
}

int Solution::sum_all_times() const
{
	int sum = 0;

	int num_drivers = get_num_drivers();
	for (int i = 0; i < num_drivers; i++)
	{
		sum += get_time_for_driver(i);
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
		if (len < 0 || len > rlen)
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

				if (!c->driver_can_service(d, stop))
				{
					err() << (*this) << std::endl;
					err() << "driver " << d << " makes stop " << stop << " which is not legal" << std::endl;
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
			log() << "the first time for driver " << d << " is bad." << std::endl;
			trap();
		}

		for (int s = 1; s < len; s++)
		{
			if (times.at(d, s) != times.at(d, s-1) + c->get_time_to(stops.at(d, s-1), stops.at(d, s)))
			{
				std::cerr << (*this) << std::endl;
				log() << "the " << s << "-th time for driver " << d << " is bad." << std::endl;
				trap();
			}
		}
	}
}

std::ostream& operator<<(std::ostream& out, const Solution& sol)
{
	if (DEBUG)
	{
		out << "indices:   ";
		for (int i = 0; i < sol.stops.cols(); i++)
		{
			out << std::setw(3) << i << ' ';
		}
		out << std::endl;
		for (int d = 0; d < sol.get_num_drivers(); d++)
		{
			out << std::setw(2) << d << " len=" << std::setw(3) << sol.lens[d] << ':';
			for (int s = 0; s < sol.stops.cols(); s++)
			{
				out << std::setw(3) << sol.get_action_index(d, s) << ' ';
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

		out << "inventories:" << std::endl;
		out << sol.invs << std::endl;

		for (int i = 0; i < sol.times.rows(); i++)
		{
			out << std::setw(5) << sol.get_time_for_driver(i) << ": ";
			out << std::setw(5) << sol.c->get_time_to(START_ACTION_INDEX, sol.stops.at(i, 0));
			for (int j = 0; j < sol.lens[i]-1; j++)
			{
				out << std::setw(5) << sol.c->get_time_to(sol.stops.at(i,j), sol.stops.at(i, j+1)) << " ";
			}
			out << std::setw(5) << sol.c->get_time_to(sol.stops.at(i,sol.lens[i]-1), END_ACTION_INDEX);
			out << std::endl;
		}
	}
	else
	{
		for (int d = 0; d < sol.get_num_drivers(); d++)
		{
			int len = sol.get_number_of_stops(d);
			for (int s = 0; s < len; s++)
			{
				out << sol.get_action_index(d, s) << ' ';
			}
			out << "| ";
		}
		out << std::endl;
	}
	out << "====================================================" << std::endl;

	return out;
}

Coord Solution::interpolate_location_at(int driver, int time, int *action) const
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
		mtime = get_time_for_driver(driver);
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
	log() << "total time: " << sum_all_times() << std::endl;
	for (int d = 0; d < get_num_drivers(); d++)
	{
		out << "driver: " << (d+1) << std::endl;
		int len = get_number_of_stops(d);
		out << "\tlength: " << len << std::endl;

		int prevloc = c->start_location;

		int t=0;
		for (int s = 0; s < len; s++)
		{
			int nloc = c->get_action(get_action_index(d, s)).location;
			t += c->durations.at(prevloc, nloc);
			out << "\t\t[stop " << std::setw(2) << (s+1) << "][t=" << std::setw(5) << t << "] stop=" << c->get_action(get_action_index(d, s)) << std::endl;
			t += c->get_action(get_action_index(d, s)).wait_time;
			prevloc = nloc;
		}
		out << std::endl;
		out << "\tend time=" << t << std::endl;
	}
}

int Solution::get_maximum_time() const
{
	int max = INT_MIN;

	int num_drivers = get_num_drivers();
	for (int i=0;i<num_drivers;i++)
	{
		max = std::max(max, get_time_for_driver(i));
	}

	return max;
}

const City* Solution::get_city() const
{
	return c;
}





















void Solution::append(int driver, int stop, int action, bool still_valid)
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
	INBOUNDS(0, action, c->num_actions);

	stops.at(driver, stop) = action;
	drivers[action] = driver;
	stop_numbers[action] = stop;
	lens[driver] = std::max(lens[driver], stop + 1);

	int prev = stop == 0 ? START_ACTION_INDEX : stops.at(driver, stop-1);
	int diff = (stop==0?0:times.at(driver, stop-1)) + c->get_time_to(prev, action) - times.at(driver, stop);
	for (int i = stop; i < lens[driver]; i++)
	{
		times.at(driver, i) += diff;
	}

	invs.action_performed(driver, stop, times.at(driver, stop), &c->get_action(action));

	if (still_valid)
	{
		ensure_valid();
	}
}

void Solution::cut(int driver, int start, int stop)
{
	trap();
}

void Solution::paste(int driver, int index, std::vector<int> path)
{
	trap();
}


void Solution::exchange(int driver1, int begin1, int end1,
		int driver2, int begin2, int end2)
{
	INBOUNDS(0, driver1, get_num_drivers());
	INBOUNDS(0, driver2, get_num_drivers());
	INBOUNDS(-1, begin1, end1+1);
	INBOUNDS(begin1, end1, lens[driver1]);
	INBOUNDS(-1, begin2, end2+1);
	INBOUNDS(begin2, end2, lens[driver2]);

//	log() << driver1 << ", " << begin1 << ", " << end1 << ", " << driver2 << ", " << begin2 << ", " << end2 << std::endl;
//	log() << "before:\n" << (*this) << std::endl;

	int len1 = 1 + end1 - begin1;
	int len2 = 1 + end2 - begin2;

	if (driver1 == driver2 && len1 != len2)
	{
		if (begin1 > begin2)
		{
			int tmp = begin1;
			begin1 = begin2;
			begin2 = tmp;

			tmp = end1;
			end1 = end2;
			end2 = tmp;
		}

		int len = end2 - begin1 + 1;
		int *t = new int[len];
		for (int i = 0; i < len; i++)
			t[i] = stops.at(driver1, begin1 + i);
		int ndx = begin1;
		for (int i = 0; i < end2 - begin2 + 1; i++)
			stops.at(driver1, ndx++) = t[begin2 - begin1 + i];
		for (int i = end1+1; i < begin2; i++)
			stops.at(driver1, ndx++) = t[i - begin1];
		for (int i = 0; i <= end1 - begin1; i++)
			stops.at(driver1, ndx++) = t[i];
		delete[] t;
		refresh();
		return;
	}

	if (len1 < len2)
	{
		int tmp = driver1;
		driver1 = driver2;
		driver2 = tmp;

		tmp = begin1;
		begin1 = begin2;
		begin2 = tmp;

		tmp = end1;
		end1 = end2;
		end2 = tmp;

		len1 = 1 + end1 - begin1;
		len2 = 1 + end2 - begin2;
	}

	for (int i = 0; i < len2; i++)
	{
		int s1 = get_action_index(driver1, begin1 + i);
		int s2 = get_action_index(driver2, begin2 + i);

		stops.at(driver1, begin1 + i) = s2;
		stops.at(driver2, begin2 + i) = s1;
	}

	int diff = len1 - len2;
	if (diff == 0)
	{
		refresh();
		return;
	}

	int newbiggestlength = lens[driver2] - 1 + diff;
	if (newbiggestlength >= stops.cols())
	{
		log() << "resizing" << std::endl;
		stops.set_num_columns(newbiggestlength + 5);
		times.set_num_columns(newbiggestlength + 5);
	}

	//  make room
	for (int i = lens[driver2] - 1 + diff; i >= begin2 + len2 + diff; i--)
		stops.at(driver2, i) = stops.at(driver2, i - diff);
	// fill in
	for (int i = 0; i < diff; i++)
		stops.at(driver2, begin2 + len2 + i) = stops.at(driver1, begin1 + len2 + i);
	// close
	for (int i = begin1 + len2; i < lens[driver1] - diff; i++)
		stops.at(driver1, i) = stops.at(driver1, i + diff);

	for (int i = lens[driver1] - diff; i < stops.cols(); i++)
	{
		if (stops.at(driver1, i) < 0)
			break;
		stops.at(driver1, i) = -1;
	}

	refresh();
}

void Solution::insert_after(int driver1, int begin1, int end1, int driver2, int begin2)
{
	INBOUNDS(0, driver1, get_num_drivers());
	INBOUNDS(0, driver2, get_num_drivers());
	INBOUNDS(-1, begin1, end1+1);
	INBOUNDS(begin1, end1, lens[driver1]);
	INBOUNDS(-1, begin2, lens[driver2]);

	log() << driver1 << ", " << begin1 << ", " << end1 << ", " << driver2 << ", " << begin2 << std::endl;
	log() << "before:\n" << (*this) << std::endl;


	int diff = end1 - begin1;

	if (driver1 == driver2)
	{
		int first, mid, end;
		if (begin2 < begin1)
		{
			first = begin2;
			mid = begin1;
			end = end1;
		}
		else
		{
			first = begin1;
			mid = end1;
			end = begin2;
		}
		int len1 = mid - first;
		int len2 = end - mid;

//		if (len1 < len2)
//		{
		int *tmp = new int[len1];
		for (int i = 0; i < len1; i++)
		{
			tmp[i] = stops.at(driver1, first + i);
		}
		for (int i = 0; i < len2; i++)
		{
			stops.at(driver1, first + i) = stops.at(driver1, first + len1 + i);
			log() << (*this) << std::endl;
		}
		for (int i = 0; i < len1; i++)
		{
			stops.at(driver1, first + len2 + i) = tmp[i];
			log() << (*this) << std::endl;
		}
		delete[] tmp;
//		}
//		else
//		{
//			... could be faster...
//		}
	}
	else
	{
		//  make room
		for (int i = lens[driver2] + diff - 1; i > begin2 + diff; i--)
		{
			stops.at(driver2, i + 1) = stops.at(driver2, i - diff);
		}
		// fill in
		for (int i = 0; i <= diff; i++)
		{
			stops.at(driver2, begin2 + 1 + i) = stops.at(driver1, begin1 + i);
		}
		// close
		for (int i = begin1; i < lens[driver1] - diff - 1; i++)
		{
			stops.at(driver1, i) = stops.at(driver1, i + diff + 1);
		}

		for (int i=lens[driver1] - diff - 1; i < stops.cols(); i++)
		{
			if (stops.at(driver1, i) < 0)
			{
				break;
			}
			stops.at(driver1, i) = -1;
		}
	}

	refresh();
}

void Solution::refresh()
{
	for (int i = 0; i < c->num_actions; i++)
	{
		drivers[i] = -1;
		stop_numbers[i] = -1;
	}

	invs.clear(c);

	int num_drivers = stops.rows();
	for (int d = 0; d < num_drivers; d++)
	{
		for (int s = 0; s < stops.cols(); s++)
		{
			int action = stops.at(d, s);
			if (action > c->num_actions)
			{
				err() << "Cannot refresh from here." << std::endl;
				err() << (*this) << std::endl;
				trap();
			}
			if (action < 0)
			{
				lens[d] = s;

				for (int i = s; i < stops.cols(); i++)
				{
					if (times.at(d, i) <= 0)
					{
						break;
					}
					times.at(d, i) = 0;
				}

				break;
			}
			if (s == 0)
			{
				times.at(d, s) = c->get_start_time(action);
			}
			else
			{
				times.at(d, s) = times.at(d, s - 1) + c->get_time_to(stops.at(d, s - 1), action);
			}
			if (c->get_action(action).value)
			{
				drivers[action] = d;
				stop_numbers[action] = s;
			}
			invs.action_performed(d, s, times.at(d, s), &c->get_action(action));
		}
	}

	ensure_valid();
}
