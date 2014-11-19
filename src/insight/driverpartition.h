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


class driver_partition
{
public:
	driver_partition();
	virtual ~driver_partition();

	int get_time_for_driver(int driver);
	std::list<interhub> *drivers;
};



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
	int num_yards;
	int num_subpaths;
	std::vector<sbp> **subpaths;

public:
	subpath_collection(const insight_state& state);
	~subpath_collection();

	void add_subpath(const interhub& subpath);

	void print_subpaths();



	friend
	void print_subpath(int start, int current, bool* already_taken, std::list<interhub>& p, const subpath_collection* collection);
};



#endif /* DRIVERPARTITION_H_ */
