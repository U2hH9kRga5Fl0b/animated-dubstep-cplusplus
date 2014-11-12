/*
 * viewer.cpp
 *
 *  Created on: Oct 31, 2014
 *      Author: thallock
 */

#include "viz/viewer.h"
#include "viz/cvvizualizer.h"

#include <map>

namespace
{
	std::map<std::string, cvvizualizer*> cvs;
}


Viewer::Viewer() : on{true} {}

Viewer::~Viewer() { turn_off(); }

void Viewer::turn_on()
{
	on = true;
}

void Viewer::turn_off()
{
	on = false;

	auto end=cvs.end();
	for (auto it = cvs.begin(); it != end; ++it)
	{
		delete it->second;
	}
	cvs.clear();
}

void Viewer::show(const std::string& name, Solution* sol)
{
	if (!on)
	{
		return;
	}

	auto it = cvs.find(name);
	if (it == cvs.end())
	{
		cvs.insert(std::pair<std::string, cvvizualizer*>{name, new cvvizualizer{name}});
		it = cvs.find(name);
	}

	it->second->show(sol);
}

void Viewer::show(const std::string& name, const City* c)
{
	if (!on)
	{
		return;
	}

	auto it = cvs.find(name);
	if (it == cvs.end())
	{
		cvs.insert(std::pair<std::string, cvvizualizer*>{name, new cvvizualizer{name}});
		it = cvs.find(name);
	}

	it->second->show(c);
}

void Viewer::snapshot(const std::string& name, const Solution* sol, const std::string& filename)
{
	if (!on)
	{
		return;
	}

	auto it = cvs.find(name);
	if (it == cvs.end())
	{
		cvs.insert(std::pair<std::string, cvvizualizer*>{name, new cvvizualizer{name}});
		it = cvs.find(name);
	}

	it->second->show(sol);
	it->second->snapshot(filename);
}

void Viewer::snapshot(const std::string& name, const City* c, const std::string& filename)
{
	if (!on)
	{
		return;
	}

	auto it = cvs.find(name);
	if (it == cvs.end())
	{
		cvs.insert(std::pair<std::string, cvvizualizer*>{name, new cvvizualizer{name}});
		it = cvs.find(name);
	}

	it->second->show(c);
	it->second->snapshot(filename);
}

void Viewer::status(const std::string& status)
{
}

#include <opencv/highgui.h>
void Viewer::pause(int length)
{
	cv::waitKey(length);
}
