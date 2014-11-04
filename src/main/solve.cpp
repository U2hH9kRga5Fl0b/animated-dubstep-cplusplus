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

#include <sstream>

void solve()
{
//	log() << *sol << std::endl;

	for (int i = 0; i < 1; i++)
	{
		Solution* sol = get_random_solution_find(city);
		std::stringstream ss;
		ss << "seed" << i;
		viewer.show(ss.str(), sol);

		while (apply_random_request_exchange(sol, 100))
			viewer.show(ss.str(), sol);

		log() << "done random sampling, enforcing..." << std::endl;

		while (apply_first_exchange(sol))
			viewer.show(ss.str(), sol);

		log() << "done enforcing, waiting..." << std::endl;
	}
	viewer.pause();
}
