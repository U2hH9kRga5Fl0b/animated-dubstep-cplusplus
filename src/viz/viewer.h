/*
 * viewer.h
 *
 *  Created on: Oct 31, 2014
 *      Author: thallock
 */

#ifndef VIEWER_H_
#define VIEWER_H_

#include "model/solution.h"

#define USE_CV 1


class Viewer
{
public:
	Viewer();
	virtual ~Viewer();

	void turn_on();
	void turn_off();

	void show(const std::string& name, Solution* sol);
	void show(const std::string& name, const City* c);

	void snapshot(const std::string& name, const Solution* sol, const std::string& filename);
	void snapshot(const std::string& name, const City* sol, const std::string& filename);

	void status(const std::string& status);
	void pause(int length=0);
private:
	bool on;
};

void get_city_bounds(const City* c, double&xmin, double&xmax, double&ymin, double&ymax);

#endif /* VIEWER_H_ */
