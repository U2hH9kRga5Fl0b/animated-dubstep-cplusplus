/*
 * solution.h
 *
 *  Created on: Oct 23, 2014
 *      Author: thallock
 */

#ifndef SOLUTION_H_
#define SOLUTION_H_

#include "model/city.h"

class solution
{
public:
	solution(const city* c, int starting_length);
	virtual ~solution();

	void service(int driver, int stop, int action);
	int get_action(int driver, int stop) const;

	int get_num_serviced() const;
	int get_time() const;

	inline int get_num_drivers() const { return stops.rows(); }
	inline int get_length(int driver) const { return lens[driver]; }

	bool already_serviced(int action) const;

	void ensure_valid() const;

	friend std::ostream& operator<<(std::ostream& out, const solution& sol);

	void human_readable(std::ostream& out) const;
private:
	const city* c;

	intarray stops;

	int *drivers;
	int *stop_numbers;

	int *lens;

//	intarray times;
//	int3array inventories;
};


#endif /* SOLUTION_H_ */
