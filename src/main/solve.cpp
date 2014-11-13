/*
 * solve.cpp
 *
 *  Created on: Oct 29, 2014
 *      Author: thallock
 */

#include "main/global.h"

#include "seed/seed.h"
#include "opts/requests_exchange.h"

#include "insight/algo.h"


#include <sstream>
#include <fstream>

void summarize(std::ostream& out, const objective* obj, const std::string& name)
{
	Solution* sol = get_random_solution_find(city);
	log () << "begin cost " << name << ": " << sol->sum_all_times() << std::endl;

	while (apply_first_exchange(sol, obj, true))
		viewer.show(name, sol);

	out << "with overtime cost: " << obj->overtime_cost;
	out << "\ndriver: \n\t" << std::endl;
	for (int i = 0; i < sol->get_num_drivers(); i++)
	{
		out << std::setw(5) << sol->get_time_for_driver(i) << ' ';
	}
	out << "\ntimes: \n\t";
	for (int i = 0; i < sol->get_num_drivers(); i++)
	{
		out << std::setw(5) << sol->get_time_for_driver(i) << ' ';
	}
	out << "\n\tsum of all times: " << sol->sum_all_times() << std::endl;
	out << "\n\tmax time: " << sol->get_maximum_time() << std::endl;

	log () << "end cost " << name << ": " << sol->sum_all_times() << std::endl;

	viewer.snapshot("local." + name, sol, name);

	delete sol;
}

void solve()
{

#if 0
	Solution* ins = do_something(city);
	viewer.show("insight seed", ins);
	viewer.snapshot("insight", ins, "after_pairing");

	{
		std::ofstream t{"after_pairing.txt"};
		ins->human_readable(t);
	}


	int total_time = ins->sum_all_times();
	log() << "be able to find this" << total_time << std::endl;

#endif

	objective tempor{0};
	Solution* sol = get_random_solution_find(city);
	while (apply_first_exchange(sol, &tempor, true))
		;
	delete do_something(city, sol);
	if (true) return;

	objective n{0};
	objective o{};
	objective n2{1};

	std::ofstream timesfile {"output/overtime_file.txt"};

	for (int i = 0; i < 1; i++)
	{
		std::stringstream ss;
		ss << "seed_" << i;

		summarize(timesfile, &o, ss.str() + "_max");
		summarize(timesfile, &n, ss.str() + "_sum");
		summarize(timesfile, &n2, ss.str() + "_overtime");
	}
	viewer.pause();
}
