/*
 * main.cpp
 *
 *  Created on: Oct 24, 2014
 *      Author: thallock
 */

#include "main/global.h"

#include "threadpool.h"

#include "insight/algo.h"
#include "c/hungarian.h"
#include "combination.h"

int main(int argc, char **argv)
{
// TODO:
//	implement maxtime
//	implement path relinking
//	finish matching
//
//	implement applying max of several possible moves
//	debug why one bad request is not moved
//	implement smart operation finder
//	implement exchanges within single path" + "
//
//	combine both exchange and reschedule
//
//	reimplement staging area capacity and truck types...


	// there is still a bug with the nearest one...
	srand(5000010);
	parse_args(argc, argv);




//	{
//		thread_pool pool;
//
//		for (int i = 0; i < 5; i++)
//		{
//			pool.add_task_to_current_group([i]() -> void { std::cout << "from thread " << i << std::endl;});
//		}
//
//		return_value *value = pool.get_next_return_value();
//		pool.finish_executing_group();
//		delete pool.get_next_return_value();
//		delete pool.get_next_return_value();
//		delete value;
//	}



	void stats();
//	stats();


//	show_robustness();

//	viewer.pause();

//	show_robustness();
	solve();

	std::cout << "done" << std::endl;

	return 0;
}


