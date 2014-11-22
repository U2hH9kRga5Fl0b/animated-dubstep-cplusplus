/*
 * driverpartition.cpp
 *
 *  Created on: Nov 18, 2014
 *      Author: thallock
 */

#include "insight/driverpartition.h"

#include "combination.h"

#include <algorithm>

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
		start_depots{new int[num_drivers]},

		assignment{new std::set<int>[num_drivers]},

		driver_times{new int[num_drivers]},
		depots_entered{new std::set<int>[num_drivers]},
		depots_exited{new std::set<int>[num_drivers]}
//		scratch_paper1{new int[collection.num_subpaths]},
//		scratch_paper2{new int[collection.num_subpaths]}
{
	for (int i = 0; i < collection.num_subpaths; i++)
	{
		assignment[rand() % num_drivers].insert(i);
		subpath_times[i] = collection.get_subpath(i).get_time();
	}

	for (int i = 0; i < num_drivers; i++)
		if ((start_depots[i] = state.embark[i].end) < 0)
			trap();

	log() << "created:\n" << *this << std::endl;
}


#define INFEASIBILITY_COST 50000


combination_partition_search::~combination_partition_search()
{
	delete[] subpath_times;
	delete[] assignment;
	delete[] start_depots;
	delete[] driver_times;
	delete[] depots_entered;
	delete[] depots_exited;
}

namespace
{
struct myfunc
{
	combination_partition_search* p;
	int driver;

	myfunc(combination_partition_search* p_, int d) : p{p_}, driver{d} {}

	void operator()(int subpath) const
	{
		p->driver_times[driver] += p->subpath_times[subpath];
		p->depots_entered[driver].insert(p->collection.get_subpath(subpath).end);
		p->depots_exited[driver].insert(p->collection.get_subpath(subpath).begin);
	}
};
}

int combination_partition_search::get_current_time()
{
	log() << "\n";



	for (int i = 0; i < num_drivers; i++)
	{
		driver_times[i] = 0;
		depots_entered[i].clear();
		depots_exited[i].clear();
		depots_entered[i].insert(start_depots[i]);
	}


	for (int driver = 0; driver < num_drivers; driver++)
	std::for_each(assignment[driver].begin(), assignment[driver].end(), myfunc{this, driver});

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
			log() << "x";
		}
	}
	log() << "\n";

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


namespace
{
void swap(std::set<int>& s1, std::set<int>&s2,
		const std::vector<int>& o1, const std::vector<int>& o2,
		const combination& c1, const combination& c2)
{
	for (int i = 0; i < c1.k; i++)
	{
		int remove = o1.at(c1.current[i]);
		s1.erase(remove);
		s2.insert(remove);
	}
	for (int i = 0; i < c2.k; i++)
	{
		int remove = o2.at(c2.current[i]);
		s2.erase(remove);
		s1.insert(remove);
	}
}
}

bool combination_partition_search::consider_all_exchanges(int max_driver, int driver2, int max_subset_size)
{
	bool improved = false;

	int m = get_current_time();
	int best_time = driver_times[m];
	std::set<int> bestd1 = assignment[max_driver];
	std::set<int> bestd2 = assignment[driver2];

	std::vector<int> orig1{assignment[max_driver].begin(), assignment[max_driver].end()};
	std::vector<int> orig2{assignment[driver2].begin(),    assignment[driver2].end()};
	int size1 = assignment[max_driver].size();
	int size2 = assignment[driver2].size();

	combination_iterator d1i { size1, std::min(size1, max_subset_size) };
	do
	{
		int time1 = 0;
		for (int i = 0; i < d1i.current().k; i++)
			time1 += driver_times[orig1.at(d1i.current().current[i])];


		combination_iterator d2i { size2, std::min(size2, max_subset_size) };
		do
		{
			int time2 = 0;
			for (int i = 0; i < d2i.current().k; i++)
				time2 += driver_times[orig2.at(d2i.current().current[i])];
			if (time2 < time1)
			{
				continue;
			}

//			log() << "running through all subsets between " << max_driver << " and " << driver2 << std::endl;
//			log() << "current combs: " << std::endl;
//			log() << d1i << "\n" << d2i << "\n";
//			log() << "before: " << *this << std::endl;

			// apply change
			swap(assignment[max_driver], assignment[driver2], orig1, orig2, d1i.current(), d2i.current());
//			log() << "between: " << *this << std::endl;

			m = get_current_time();
			int time = driver_times[m];
			if (time < best_time)
			{
				best_time = time;
				bestd1 = assignment[max_driver];
				bestd2 = assignment[driver2];
				improved = true;
			}

			// undo change
			swap(assignment[driver2], assignment[max_driver], orig1, orig2, d1i.current(), d2i.current());
//			log() << "after: " << *this << std::endl;
		} while (d2i.increment());
	} while (d1i.increment());

	assignment[max_driver] = bestd1;
	assignment[driver2] = bestd2;

	return improved;
}

void combination_partition_search::search()
{
//	log() << "collection: " << collection << std::endl;
	log() << "start:" << *this << std::endl;
	get_current_time();

	for (int max_subset_size = 1; max_subset_size < 3; max_subset_size++)
	{
		bool improved = true;
		while (improved)
		{
			improved = false;
			int max_driver = get_current_time();
			for (int i = 0; i < num_drivers; i++)
			{
				if (i == max_driver)
				{
					continue;
				}
				if (consider_all_exchanges(max_driver, i, max_subset_size))
				{
					improved = true;
					break;
				}
			}
		}
	}
	log() << "end:" << *this << std::endl;
	get_current_time();
}

std::ostream& operator<<(std::ostream& out, const combination_partition_search& search)
{
	out << '\n';
	for (int i = 0; i < search.num_drivers; i++)
	{
		out << "[" << std::setw(5) << i << ":";
		auto end = search.assignment[i].end();
		for (auto it = search.assignment[i].begin(); it != end; ++it)
		{
			out << std::setw(5) << *it << " ";
		}
		out << "]";
	}
	return out;
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
