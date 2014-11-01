/*
 * make_video.cpp
 *
 *  Created on: Oct 29, 2014
 *      Author: thallock
 */

#include "main/global.h"
#include "seed/seed.h"
#include "viz/cvvizualizer.h"

void make_video()
{
	Solution* sol = get_nearest_seed(city);

	cvvizualizer v { "foobar" };
	v.show(sol);
	v.pause(1000);

	int last_time = sol->get_maximum_time() + 10 * 60;
	for (int t = 0; t < last_time; t += 20)
	{
		v.show(sol, t);
	}
}
