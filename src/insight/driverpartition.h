/*
 * driverpartition.h
 *
 *  Created on: Nov 18, 2014
 *      Author: thallock
 */

#ifndef DRIVERPARTITION_H_
#define DRIVERPARTITION_H_

#include "interhub.h"
#include "insight_state.h"


#include "combination.h"

#include <set>

class combination_partition_search;

struct sbp
{
	int id;
	interhub path;
	sbp(int id_, interhub hubs_) :
		id{id_},
		path{hubs_} {}
};

class subpath_collection
{
	friend combination_partition_search;
	int num_yards;
	int num_subpaths;
	std::vector<sbp> **subpaths;
	std::vector<interhub> another_copy;

public:
	subpath_collection(const insight_state& state);
	~subpath_collection();

	void add_subpath(const interhub& subpath);

	void print_subpaths();
	interhub get_subpath(int index) const;

	friend
	void print_subpath(int start, int current, bool* already_taken, std::list<interhub>& p, const subpath_collection* collection);
};

class combination_partition_search
{
	const subpath_collection collection;
	int num_drivers;

	int *subpath_times;
	int *subpath_drivers;
	int *start_depots;
	int *driver_times;

	std::set<int> *depots_entered;
	std::set<int> *depots_exited;
public:
	combination_partition_search(const insight_state& state);
	~combination_partition_search();

	int get_current_time();

	bool consider_exchanging(int driver1, int driver2);

	void search();
};




#endif /* DRIVERPARTITION_H_ */
