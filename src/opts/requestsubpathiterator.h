/*
 * requestsubpathiterator.h
 *
 *  Created on: Nov 7, 2014
 *      Author: thallock
 */

#ifndef REQUESTSUBPATHITERATOR_H_
#define REQUESTSUBPATHITERATOR_H_

#include "model/solution.h"

class driver_subpath
{
public:
	int driver, begin, end;
	const Solution* solution;

	driver_subpath(const Solution *solution);

	void set_solution(const Solution* solution);

	bool reset();
	bool increment();
	bool set_greater_than(const driver_subpath& other);

	bool is_valid() const;
	bool find_driver();
	bool follows(const driver_subpath& other) const;

	bool const operator==(const driver_subpath& other) const
	{
		return driver == other.driver && begin == other.begin && end == other.end;
	}
};


class solution_exchange_iterator
{
public:
	driver_subpath sub1;
	driver_subpath sub2;

private:
	const Solution* solution;

	driver_subpath lastchange1;
	driver_subpath lastchange2;
public:
	solution_exchange_iterator(const Solution *solution);

	void set_solution(const Solution* solution);

	void reset();
	void increment();
	void solution_changed();

	bool is_valid() const;

	bool at_last_change() const;

private:
	void reset_last_changed();
};

#endif /* REQUESTSUBPATHITERATOR_H_ */
