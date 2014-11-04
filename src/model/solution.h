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
#include "model/truckstate.h"

class Solution
{
public:
	Solution(const City* c, int starting_length);
	virtual ~Solution();

	Solution& operator=(const Solution& other);
	Solution(const Solution& other);

	int get_action_index(int driver, int stop) const;
	bool already_serviced(int action) const;
	const City* get_city() const;

	int get_num_serviced() const;
	int get_time_for_driver(int driver) const;
	int sum_all_times() const;
	int get_maximum_time() const;

	inline int get_num_drivers() const { return stops.rows(); }
	inline int get_number_of_stops(int driver) const { return lens[driver]; }
	inline int get_current_dimension() {return stops.cols(); }

	Coord interpolate_location_at(int driver, int time, int *action) const;

	void ensure_valid() const;
	friend bool is_feasible(const Solution* solution);

	friend std::ostream& operator<<(std::ostream& out, const Solution& sol);
	void human_readable(std::ostream& out) const;

	// operations...
	void append(int driver, int stop, int action, bool still_valid=true);
	void exchange(int driver1, int begin1, int end1, int driver2, int begin2, int end2);
	void insert_after(int driver1, int begin1, int end1, int driver2, int begin2);

	// does not call is_valid()...
	void set_action(int driver, int stop, int action_index) { stops.at(driver, stop) = action_index; }
	void shift(int driver, int stop, int shift_amount);
	void refresh();


	void debug_stops() { std::cout << stops << std::endl; }
private:
	const City* c;
	intarray stops;

	int *drivers;
	int *stop_numbers;

	int *lens;
	intarray times;
	InventoryTimeline invs;
};


void take_still_shot(const Solution* solution, int time, Coord *coords, int *actions);

#endif /* SOLUTION_H_ */
