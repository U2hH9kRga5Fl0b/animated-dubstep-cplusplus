/*
 * requestsubpathiterator.cpp
 *
 *  Created on: Nov 7, 2014
 *      Author: thallock
 */

#include "opts/requestsubpathiterator.h"

namespace
{
bool find_req(const Solution* solution, int driver, int& index)
{
	const City* city = solution->get_city();
	for (;; index++)
	{
		if (index > solution->get_number_of_stops(driver))
		{
			return false;
		}
		if (city->get_action(solution->get_action_index(driver, index)).value)
		{
			return true;
		}
	}
	return false;
}
}

driver_subpath::driver_subpath(const Solution* solution_) : driver{0}, begin{0}, end{0}, solution{solution_} {};



bool driver_subpath::reset()
{
	driver = 0;

	begin = 0;
	if (find_req(solution, driver, begin))
	{
		end = begin + 1;
		if (find_req(solution, driver, end))
		{
			return true;
		}
	}

	return find_driver();
}

bool driver_subpath::set_greater_than(const driver_subpath& other)
{
	driver = other.driver;

	begin = other.end + 1;
	if (find_req(solution, driver, begin))
	{
		end = begin + 1;
		if (find_req(solution, driver, end))
		{
			return true;
		}
	}

	return find_driver();
}

bool driver_subpath::find_driver()
{
	const City* city = solution->get_city();
	while (++driver < city->num_trucks)
	{
		begin = 0;
		if (!find_req(solution, driver, begin))
		{
			continue;
		}
		end = begin + 1;
		if (!find_req(solution, driver, begin))
		{
			continue;
		}
		return true;
	}
	return false;
}

bool driver_subpath::increment()
{
	for(;;)
	{
		end++;

		if (find_req(solution, driver, end))
			return true;

		if (find_req(solution, driver, begin))
		{
			end = begin;
			continue;
		}

		return find_driver();
	}
	return false;
}

bool driver_subpath::is_valid() const
{
	const City* city = solution->get_city();
	if (driver >= solution->get_city()->num_trucks)
		return false;
	int len = solution->get_number_of_stops(driver);
	if (begin >= len)
		return false;
	if (!city->get_action(solution->get_action_index(driver, begin)).value)
		return false;
	if (end >= len)
		return false;
	if (end <= begin)
		return false;
	if (!city->get_action(solution->get_action_index(driver, end)).value)
		return false;
	return true;
}

bool solution_exchange_iterator::is_valid() const
{
	return sub1.is_valid() && sub2.is_valid() && sub2.follows(sub1);
}

bool driver_subpath::follows(const driver_subpath& other) const
{
	if (driver > other.driver)
	{
		return true;
	}

	return begin > other.end;
}


























solution_exchange_iterator::solution_exchange_iterator(const Solution* solution_) :
		sub1        {solution_},
		sub2        {solution_},
		solution    {solution_},
		lastchange1 {solution_},
		lastchange2 {solution_}
{
	reset();
}

void solution_exchange_iterator::reset()
{
	sub1.reset();
	sub2.set_greater_than(sub1);
}


void solution_exchange_iterator::increment()
{
	if (sub2.increment())
	{
		return;
	}
	if (sub1.increment())
	{
		if (sub2.set_greater_than(sub1))
		{
			return;
		}
	}
	sub1.reset();
	sub2.set_greater_than(sub1);
}

void solution_exchange_iterator::solution_changed()
{
	if (sub1.is_valid())
	{
		if (sub2.is_valid())
		{
			reset_last_changed();
			return;
		}
		if (sub2.set_greater_than(sub1))
		{
			reset_last_changed();
			return;
		}
	}
	if (!sub1.find_driver() || !sub2.set_greater_than(sub1))
	{
		sub1.reset();
		sub2.set_greater_than(sub1);
	}

	reset_last_changed();
}

void solution_exchange_iterator::reset_last_changed()
{
	lastchange1 = sub1;
	lastchange2 = sub2;
}

bool solution_exchange_iterator::at_last_change() const
{
	return sub1 == lastchange1 && sub2 == lastchange2;
}
