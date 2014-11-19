/*
 * driverpartition.cpp
 *
 *  Created on: Nov 18, 2014
 *      Author: thallock
 */

#include <insight/driverpartition.h>

driver_partition::driver_partition() {}
driver_partition::~driver_partition() {}


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
}

subpath_collection::~subpath_collection()
{
	for (int i = 0; i < num_yards; i++)
		delete[] subpaths[i];
	delete[] subpaths;
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
