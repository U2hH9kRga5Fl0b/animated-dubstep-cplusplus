/*
 * solve.cpp
 *
 *  Created on: Oct 29, 2014
 *      Author: thallock
 */

#include "main/global.h"

#include "seed/seed.h"
#include "opts/codon.h"
#include "opts/requests_exchange.h"

#include "insight/algo.h"

#include <sstream>
#include <fstream>

void solve()
{
//	log() << *sol << std::endl;

	Solution* ins = do_something(city);
	viewer.show("insight seed", ins);
	viewer.snapshot("insight", ins, "after_pairing");

	{
		std::ofstream t{"after_pairing.txt"};
		ins->human_readable(t);
	}


	int total_time = ins->sum_all_times();
	log() << "be able to find this" << total_time << std::endl;

	for (int i = 0; i < 1; i++)
	{
		std::stringstream ss;
		ss << "seed" << i;
		viewer.show(ss.str(), ins);

		while (apply_random_request_exchange(ins, 100))
			viewer.show(ss.str(), ins);

		log() << "done random sampling, enforcing..." << std::endl;

		while (apply_first_exchange(ins))
			viewer.show(ss.str(), ins);

		viewer.snapshot("local", ins, "after_local_search");
		log() << "done enforcing, waiting..." <<  ins->sum_all_times() << std::endl;
	}
	viewer.pause();

	int tt = 0;
	for (int i=0;i<10;i++)
		tt += get_random_solution_find(city)->sum_all_times();
	log() << "avg random: " << tt << std::endl;
}
