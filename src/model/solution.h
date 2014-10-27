/*
 * solution.h
 *
 *  Created on: Oct 23, 2014
 *      Author: thallock
 */

#ifndef SOLUTION_H_
#define SOLUTION_H_

#include "model/city.h"
#include "model/inventory.h"

class Solution
{
public:
	Solution(const City* c, int starting_length);
	virtual ~Solution();

	void service(int driver, int stop, int action);
	int get_action(int driver, int stop) const;

	int get_num_serviced() const;
	int get_time_for(int driver) const;
	int get_time() const;
	bool already_serviced(int action) const;

	inline int get_num_drivers() const { return stops.rows(); }
	inline int get_length(int driver) const { return lens[driver]; }

	Coord get_location_at(int driver, int time, int *action) const;

	void ensure_valid() const;

	friend std::ostream& operator<<(std::ostream& out, const Solution& sol);

	void human_readable(std::ostream& out) const;
	int get_last_time() const;

	const City* c;
private:

	intarray stops;

	int *drivers;
	int *stop_numbers;

	int *lens;

	intarray times;
	InventoryTimeline invs;
};


void take_still_shot(const Solution* solution, int time, Coord *coords, int *actions);

#endif /* SOLUTION_H_ */
