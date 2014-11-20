/*
 * driverpartition.cpp
 *
 *  Created on: Nov 18, 2014
 *      Author: thallock
 */

#include "insight/driverpartition.h"

subpath_collection::subpath_collection(const insight_state& state) :
	num_yards{state.info->num_staging_areas},
	num_subpaths{0},
	subpaths{new std::vector<sbp>*[num_yards]}
{
	for (int i = 0; i < state.info->num_staging_areas; i++)
	{
		subpaths[i] = new std::vector<sbp>[num_yards];
	}
	bool *aser = new bool[state.info->pickup_lens];
	for (int i = 0; i < state.info->pickup_lens; i++)
		aser[i] = false;

	for (int i = 0; i < state.info->deliver_lens; i++)
	{
		interhub current;
		current.begin = state.deliver_depots[i];
		current.path[0] = state.info->deliver_actions[i];
		int pickup = state.delivers_to_pickups[i];
		if (pickup >= 0)
		{
			current.path[1] = state.info->pickup_actions[pickup];
			current.end = state.pickup_depots[pickup];
			aser[pickup] = true;
		}
		else
		{
			current.path[1] = -1;
			current.end = state.unmatched_delivers[i];
		}
		add_subpath(current);
	}

	for (int i = 0; i < state.info->pickup_lens; i++)
	{
		int pickup_depot = state.unmatched_pickups[i];
		if (pickup_depot < 0)
		{
			continue;
		}

		if (aser[i])
		{
			trap();
		}

		interhub current;
		current.begin = state.pickup_depots[i];
		current.path[0] = state.info->pickup_actions[i];
		current.end = pickup_depot;

		add_subpath(current);
	}

	delete[] aser;
}

void subpath_collection::add_subpath(const interhub& subpath)
{
	subpaths[subpath.begin][subpath.end].push_back(sbp{num_subpaths++, subpath});
	another_copy.push_back(subpath);
}

subpath_collection::~subpath_collection()
{
	for (int i = 0; i < num_yards; i++)
		delete[] subpaths[i];
	delete[] subpaths;
}

interhub subpath_collection::get_subpath(int index) const
{
	return another_copy.at(index);
}

void print_subpath(int start, int current, bool* already_taken, std::list<interhub>& p, const subpath_collection* collection)
{
	if (start == current)
	{
		auto end = p.end();
		for (auto it = p.begin(); it != end; ++it)
		{
			log() << "["	<< std::setw(4) << it->begin
					<< std::setw(4) << it->path[0]
					<< std::setw(4) << it->path[1]
					<< std::setw(4) << it->end << "]";
		}
		log() << std::endl;
		return;
	}

	if (current < 0)
	{
		current = start;
	}

	for (int i = 0; i < collection->num_yards; i++)
	{
		if (i == current)
		{
			continue;
		}

		int size = collection->subpaths[current][i].size();
		for (int j = 0; j < size; j++)
		{
			int id = collection->subpaths[current][i].at(j).id;
			if (already_taken[id])
			{
				continue;
			}
			int next = collection->subpaths[current][i].at(j).path.end;
			already_taken[id] = true;
			p.push_back(collection->subpaths[current][i].at(j).path);
			print_subpath(start, next, already_taken, p, collection);
			p.pop_back();
			already_taken[id] = false;
		}
	}
}

void subpath_collection::print_subpaths()
{
	bool* already_taken = new bool[num_subpaths];
	for (int k = 0; k < num_subpaths; k++)
		already_taken[k] = false;
	std::list<interhub> p;
	for (int i = 0; i < num_yards; i++)
		print_subpath(i, -1, already_taken, p, this);
	delete[] already_taken;
}



























combination_partition_search::combination_partition_search(const insight_state& state) :
		collection{state},
		num_drivers{state.info->city->num_trucks},
//		num_yards{collection.num_yards},
//		num_subpaths{collection.num_subpaths},

		subpath_times{new int[collection.num_subpaths]},
		subpath_drivers{new int[collection.num_subpaths]},
		start_depots{new int[num_drivers]},


		driver_times{new int[num_drivers]},
		depots_entered{new std::set<int>[num_drivers]},
		depots_exited{new std::set<int>[num_drivers]}
{
	for (int i = 0; i < collection.num_subpaths; i++)
	{
		subpath_drivers[i] = rand() % num_drivers;
		subpath_times[i] = collection.get_subpath(i).get_time();
	}

	for (int i = 0; i < num_drivers; i++)
		if ((start_depots[i] = state.embark[i].end) < 0)
			trap();

}


#define INFEASIBILITY_COST 500


combination_partition_search::~combination_partition_search()
{
	delete[] subpath_times;
	delete[] subpath_drivers;
	delete[] start_depots;
	delete[] driver_times;
	delete[] depots_entered;
	delete[] depots_exited;
}

int combination_partition_search::get_current_time()
{
	for (int i = 0; i < num_drivers; i++)
	{
		driver_times[i] = 0;
		depots_entered[i].clear();
		depots_exited[i].clear();
		depots_entered[i].insert(start_depots[i]);
	}


	for (int i = 0; i < collection.num_subpaths; i++)
	{
		const int driver = subpath_drivers[i];
		driver_times[driver] += subpath_times[i];
		depots_entered[driver].insert(collection.get_subpath(i).end);
		depots_exited[driver].insert(collection.get_subpath(i).begin);
	}


	for (int i = 0; i < num_drivers; i++)
	{
		auto end = depots_exited[i].end();
		for (auto it = depots_exited[i].begin(); it != end; ++it)
		{
			int depot = *it;
			if (depots_entered[i].find(depot) != depots_entered[i].end())
				continue;

			// should be smarter, like find closest depot visited...
			driver_times[i] += INFEASIBILITY_COST;
		}
	}

	int max = 0;
	int max_driver = 0;
	for (int i = 0; i < num_drivers; i++)
	{
		if (driver_times[i] > max)
		{
			max = driver_times[i];
			max_driver = i;
		}
	}
	return max_driver;
}

#define MAX_SUBSIZE 5

bool combination_partition_search::consider_exchanging(int max_driver, int driver2)
{






	// consider swapping...
	combination comb { 25, 10, false };
	do
	{
		std::cout << comb << std::endl;
	}
	while (comb.increment());

	return false;
}

void combination_partition_search::search()
{
	bool improved = true;
	while (improved)
	{
		improved = false;
		int max_driver = get_current_time();
		for (int i = 0; i < num_drivers; i++)
		if (consider_exchanging(max_driver, i))
		{
			improved = true;
			break;
		}
	}
}



#if 0

struct subset_tally
{
	int num_depots;
	int *in;
	int *out;

	subset_tally(int num_depots_) : num_depots{num_depots_}, in{new int[num_depots]}, out{new int[num_depots]} {}
	~subset_tally() { delete[] in; delete[] out; }

	void tally(uint32_t subset, const std::list<interhub>& l);
};

struct subset_iterator
{
	uint32_t subset;
	uint32_t
	const std::list<interhub>& partition;
	subset_tally tally;

	subset_iterator(const std::list<interhub>& partition, int ndepots) :
		subset{0},




	bool increment();
	bool in_bounds(driver_partition& partition);
	bool compatible(const subset_iterator& other);
	int get_improvement(subset_iterator other, n)

};
#endif
