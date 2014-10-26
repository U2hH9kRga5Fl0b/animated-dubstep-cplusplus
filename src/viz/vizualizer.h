/*
 * vizualizer.h
 *
 *  Created on: Oct 25, 2014
 *      Author: thallock
 */

#ifndef VIZUALIZER_H_
#define VIZUALIZER_H_

#include "model/solution.h"

class vizualizer
{
public:
	vizualizer(const std::string& name);
	virtual ~vizualizer();

	void pause(int length=0);
	void show(const Solution* sol);
	void show(const Solution* sol, int time);
	void show(const City* c);

	void snapshot(std::string file);

	// don't touch
	void* internal_state;
private:
	void write_frame_of_video();
};

#endif /* VIZUALIZER_H_ */
