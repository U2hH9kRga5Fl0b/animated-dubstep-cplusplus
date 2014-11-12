/*
 * neighbor.h
 *
 *  Created on: Nov 8, 2014
 *      Author: thallock
 */

#ifndef NEIGHBOR_H_
#define NEIGHBOR_H_

#include "model/solution.h"

#define VERBOSE_NEIGHBOR_OPERATIONS 1

enum objective_type
{
	relink,
	times_sum,
	max_time,
};

class  objective
{
public:
	objective_type type;
	// union {
		// if relink
		const Solution* goal;
		// if times_sum
		int overtime_cost;
	// }
	objective(const Solution* goal_) : type{relink}, goal{goal_}, overtime_cost{0} {}
	objective(int overtime) : type{times_sum}, goal{nullptr}, overtime_cost{overtime} {}
	objective() : type{max_time}, goal{nullptr}, overtime_cost{0} {}
};

inline bool IS_REQUEST(const Solution* solution, int d, int s)
{
	return d < solution->get_num_drivers()
			&& s < solution->get_number_of_stops(d)
			&& solution->get_city()->get_action(solution->get_action_index(d, s)).value;
}

int consider_exchange(Solution* s,
		int d1, int b1, int e1,
		int d2, int b2, int e2,
		const objective *obj, bool apply=true);

int consider_reschedule(Solution* s,
		int d1, int b1, int e1,
		int d2, int b2,
		const objective *obj, bool apply=true);

#endif /* NEIGHBOR_H_ */
