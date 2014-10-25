/*
 * vizualizer.cpp
 *
 *  Created on: Oct 25, 2014
 *      Author: thallock
 */

#include "viz/vizualizer.h"

#define USE_BACKGROUND_IMAGE 1

#if USE_BACKGROUND_IMAGE
#define BACKGROUND_IMAGE "background.jpg"
#else
#define default_width  500
#define default_height 500
#endif
#define image_update_time 20
#define SPACER

#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>

#include <fstream>

#include <chrono>
#include <mutex>

namespace
{
	std::mutex graphics_mutex;

	class state
	{
	public:
		state(const std::string& name_) :
			name {name_},
			start { std::chrono::steady_clock::now() },
#if USE_BACKGROUND_IMAGE
			mat{cv::imread(BACKGROUND_IMAGE, CV_LOAD_IMAGE_COLOR)},
#else
			mat{cv::Mat::zeros(default_height, default_width, CV_8UC3)},
#endif
			colors {},
			vid { "output/" + name + ".mpeg", CV_FOURCC('P', 'I', 'M', '1'), 120, cv::Size(mat.rows, mat.cols) },
			log { "output/" + name + ".txt" },
			mut{}
		{
			cvNamedWindow(name.c_str(), CV_WINDOW_AUTOSIZE);
			cvMoveWindow(name.c_str(), 100,100);
		}
		~state() {}

		cv::Scalar& get_color(int ndx)
		{
			while ((int) colors.size() <= ndx)
			{
				colors.push_back(cv::Scalar { 255 * (rand() / (double) RAND_MAX), 255 * (rand() / (double) RAND_MAX), 255 * (rand() / (double) RAND_MAX) });
			}
			return colors.at(ndx);
		}

		std::string name;
		std::chrono::steady_clock::time_point start;
		cv::Mat mat;
		std::vector<cv::Scalar> colors;
		cv::VideoWriter vid;
		std::ofstream log;
		std::mutex mut;
	};


	state* get_state(void* viz)
	{
		return (state*) viz;
	}

	void get_bounds(const city* c, double&xmin, double&xmax, double&ymin, double&ymax)
	{
		xmin = ymin =  DBL_MAX;
		xmax = ymax = -DBL_MAX;
		for (int i = 0; i < c->num_locations; i++)
		{
			if (c->coords[i].x < xmin)
			{
				xmin = c->coords[i].x;
			}
			if (c->coords[i].x > xmax)
			{
				xmax = c->coords[i].x;
			}
			if (c->coords[i].y < ymin)
			{
				ymin = c->coords[i].y;
			}
			if (c->coords[i].y > ymax)
			{
				ymax = c->coords[i].y;
			}
		}
	}

	void clear_mat(state* istate)
	{
#if USE_BACKGROUND_IMAGE
		istate->mat = cv::imread(BACKGROUND_IMAGE, CV_LOAD_IMAGE_COLOR);
#else
		istate->mat = cv::Scalar(0, 0, 0);
#endif
	}

	void print_sol_to_mat(const solution* sol, state* istate)
	{
		double minx, miny, maxx, maxy;
		get_bounds(sol->c, minx, maxx, miny, maxy);

		// std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count()

		for (int d=0;d<sol->get_num_drivers();d++)
		{
			int length = sol->get_length(d);
			if (length <= 0)
			{
				continue;
			}
			cv::Scalar& color = istate->get_color(d);

			// draw first line
			coord& c = sol->c->coords[sol->c->start_location];
			double x = istate->mat.cols * (c.x - minx) / (maxx - minx);
			double y = istate->mat.rows * (c.y - miny) / (maxy - miny);
			cv::Point prev((int) c.x, (int) c.y);
			cv::circle(istate->mat, prev, 3, color);

			// draw middle lines
			for (int s = 0; s < length; s++)
			{
				int loc = sol->c->actions[sol->get_action(d, s)].location;
				x = istate->mat.cols * (sol->c->coords[loc].x - minx) / (maxx - minx);
				y = istate->mat.rows * (sol->c->coords[loc].y - miny) / (maxy - miny);

				cv::Point next((int) x, (int) y);
				cv::circle(istate->mat, next, 3, color);
				cv::line(istate->mat, prev, next, color, 1, 8, 0);

				prev = next;
			}

			// draw last line
			c = sol->c->coords[sol->c->start_location];
			x = istate->mat.cols * (c.x - minx) / (maxx - minx);
			y = istate->mat.rows * (c.y - miny) / (maxy - miny);
			cv::Point next((int) x, (int) y);
			cv::line(istate->mat, prev, next, color, 1, 8, 0);
		}

		std::stringstream s;
		s << "n=" << sol->get_num_serviced() << " t=" << sol->get_time();

		cv::Scalar invert = istate->get_color(sol->get_num_drivers() + 1);

		cv::putText(istate->mat, s.str(), cv::Point(0,20), CV_FONT_HERSHEY_PLAIN, 1.0, invert);
		istate->vid.write(istate->mat);
	}

	void print_city_to_mat(const city* c, state* istate)
	{
		double minx, miny, maxx, maxy;
		get_bounds(c, minx, maxx, miny, maxy);

		cv::Scalar& color = istate->get_color(0);

		for (int i = 0; i < c->num_locations; i++)
		{
			double x = istate->mat.cols * (c->coords[i].x - minx) / (maxx - minx);
			double y = istate->mat.rows * (c->coords[i].y - miny) / (maxy - miny);

			cv::Point prev((int) x, (int) y);
			cv::circle(istate->mat, prev, 3, color);
		}
	}
}

vizualizer::vizualizer(const std::string& name)
{
	internal_state = new state(name);
}

vizualizer::~vizualizer()
{
	delete get_state(internal_state);
}

void vizualizer::show(const solution* sol)
{
	std::lock_guard<std::mutex> lock{get_state(internal_state)->mut};
	clear_mat(get_state(internal_state));
	print_city_to_mat(sol->c, get_state(internal_state));
	print_sol_to_mat(sol, get_state(internal_state));

	std::lock_guard<std::mutex> global{graphics_mutex};
	imshow(get_state(internal_state)->name, get_state(internal_state)->mat);
	cv::waitKey(image_update_time);
}

void vizualizer::show(const city* c)
{
	std::lock_guard<std::mutex> lock{get_state(internal_state)->mut};
	clear_mat(get_state(internal_state));
	print_city_to_mat(c, get_state(internal_state));

	std::lock_guard<std::mutex> global{graphics_mutex};
	imshow(get_state(internal_state)->name, get_state(internal_state)->mat);
	cv::waitKey(image_update_time);
}

void vizualizer::pause(int length)
{
	cv::waitKey(length);
}

void vizualizer::snapshot(std::string file)
{
	std::lock_guard<std::mutex> lock{get_state(internal_state)->mut};
	std::lock_guard<std::mutex> global{graphics_mutex};
	cv::imwrite(get_state(internal_state)->name, get_state(internal_state)->mat);
}
