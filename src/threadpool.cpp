/*
 * threadpool.cpp
 *
 *  Created on: Nov 9, 2014
 *      Author: thallock
 */

#include "threadpool.h"

#include <algorithm>

#define POLL_TIME 1000


#define ASSERT_STATE(state)                                     \
do                                                              \
{                                                               \
	if (current_state == state) break;                      \
	err() << "Bad thread pool state. expected: " << state   \
	      << " found: " << current_state << std::endl;      \
	trap();                                                 \
} while(0)


#define ASSERT_STATES(state1, state2)                           \
do                                                              \
{                                                               \
	if (current_state == state1) break;                     \
	if (current_state == state2) break;                     \
	err() << "Bad thread pool state. expected: " << state1  \
	      << " found: " << current_state << std::endl;      \
	trap();                                                 \
} while(0)

#define IS_STATE(state) (current_state == (state))
#define IN_STATES(state1, state2) (current_state == (state1) || current_state == (state2))

thread_pool::thread_pool(int num_threads) :
	nthreads{num_threads},
	nbusy{0},
	pool_mutex{},
	cv{},
	threads{new std::thread*[nthreads]},
	return_values{},
	work_queue{},
	current_state{STATE_EXECUTING}
{
	for (int i = 0; i < nthreads; i++)
	{
		threads[i] = new std::thread{[this](){thread_pool_loop();}};
	}
}

thread_pool::~thread_pool()
{
	std::unique_lock<std::mutex> lk{pool_mutex};
	ASSERT_STATE(STATE_READING);

	current_state = STATE_QUITING;
	for (int i = 0; i < nthreads; i++)
	{
		threads[i]->join();
		delete threads[i];
	}
	std::for_each(return_values.begin(), return_values.end(), [](return_value* value){ delete value; });
}

void thread_pool::add_task_to_current_group(task& t)
{
	std::unique_lock<std::mutex> lk{pool_mutex};
	ASSERT_STATES(STATE_EXECUTING, STATE_FINISHING);

	work_queue.push_back(t);

	lk.unlock();
	cv.notify_all();
}

return_value* thread_pool::get_next_return_value()
{
	std::lock_guard<std::mutex> lock{pool_mutex};

	if (	   current_state != STATE_EXECUTING
		&& current_state != STATE_FINISHING
		&& current_state != STATE_READING)
	{
		trap();
	}

	if (return_values.size() == 0)
	{
		return nullptr;
	}

	return_value* ret = return_values.front();
	return_values.pop_front();

	return ret;
}

void thread_pool::finish_executing_group()
{
	std::unique_lock<std::mutex> lk{pool_mutex};

	ASSERT_STATE(STATE_EXECUTING);
	current_state = STATE_FINISHING;

	cv.wait(lk, [this]()
	{
		ASSERT_STATE(STATE_FINISHING);
		return nbusy == 0 && work_queue.size() == 0;
	});

	ASSERT_STATE(STATE_FINISHING);
	current_state = STATE_READING;

	lk.unlock();
	cv.notify_all();
}

void thread_pool::start_next_group()
{
	std::unique_lock<std::mutex> lk{pool_mutex};
	ASSERT_STATE(STATE_READING);
	if (work_queue.size() != 0 || nbusy != 0)
	{
		trap();
	}
	current_state = STATE_EXECUTING;
	std::for_each(return_values.begin(), return_values.end(), [](return_value* value){ delete value; });
	return_values.clear();
}

void thread_pool::add_task_to_current_group(std::function<void(void)>&& t)
{
	add_task_to_current_group([&t](){ t(); return new return_value;});
}

void thread_pool::thread_pool_loop()
{
	std::unique_lock<std::mutex> lk{pool_mutex};
	for(;;)
	{
		cv.wait(lk, [this]()
		{
			return current_state == STATE_QUITING || work_queue.size() > 0;
		});

		if (current_state == STATE_QUITING)
		{
			break;
		}
		task t = work_queue.front();
		work_queue.pop_front();
		nbusy++;

		lk.unlock();
		cv.notify_all();

		return_value* ret = t();

		lk.lock();

		return_values.push_back(ret);
		nbusy--;
	}

	lk.unlock();
	cv.notify_all();
}
