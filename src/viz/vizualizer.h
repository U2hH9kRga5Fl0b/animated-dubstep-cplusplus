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
	void show(const solution* sol);
	void show(const city* c);
	void snapshot(std::string file);

	// don't touch
	void* internal_state;
};

#endif /* VIZUALIZER_H_ */
