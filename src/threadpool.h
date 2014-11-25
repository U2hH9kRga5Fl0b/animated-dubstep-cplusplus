/*
 * threadpool.h
 *
 *  Created on: Nov 9, 2014
 *      Author: thallock
 */

#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include "common.h"


#include <list>
#include <mutex>
#include <thread>
#include <functional>
#include <condition_variable>

class return_value
{
public:
	return_value() {}
	virtual ~return_value() {}
};

typedef std::function<return_value*(void)> task;

enum thread_pool_state
{
	STATE_EXECUTING,
	STATE_FINISHING,
	STATE_READING,
	STATE_QUITING,
};

class thread_pool
{
private:
	int nthreads;
	int nbusy;

	std::mutex pool_mutex;
	std::condition_variable cv;

	std::thread ** threads;

	std::list<return_value *> return_values;
	std::list<task> work_queue;

	thread_pool_state current_state;
public:
	thread_pool(int num_threads=10);
	~thread_pool();

	void add_task_to_current_group(task& t);
	void add_task_to_current_group(std::function<void(void)>&& t);
	void finish_executing_group();
	return_value *get_next_return_value();
	void start_next_group();



	void thread_pool_loop();
};


#endif /* THREADPOOL_H_ */
