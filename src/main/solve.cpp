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

	objective n{0};
	while (apply_first_exchange(sol, &n, true))
		viewer.show(name, sol);
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

	objective obj1 { 0};
	objective obj8 { 1};
	objective obj2 { 2};
	objective obj3 { 3};
	objective obj4 { 4};
	objective obj5 { 5};
	objective obj6 {10};
	objective obj7 {50};

	std::ofstream timesfile {"overtime_file.txt"};

	for (int i = 0; i < 1; i++)
	{
		std::stringstream ss;
		ss << "seed" << i;

		summarize(timesfile, &obj1, ss.str());
		summarize(timesfile, &obj8, ss.str());
		summarize(timesfile, &obj2, ss.str());
		summarize(timesfile, &obj3, ss.str());
		summarize(timesfile, &obj4, ss.str());
		summarize(timesfile, &obj5, ss.str());
		summarize(timesfile, &obj6, ss.str());
		summarize(timesfile, &obj7, ss.str());
	}
	viewer.pause();
}
