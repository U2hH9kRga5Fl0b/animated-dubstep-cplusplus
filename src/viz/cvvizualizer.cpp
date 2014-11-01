/*
 * vizualizer.cpp
 *
 *  Created on: Oct 25, 2014
 *      Author: thallock
 */

#include "viz/cvvizualizer.h"

#define USE_BACKGROUND_IMAGE 0

#if USE_BACKGROUND_IMAGE
#define BACKGROUND_IMAGE "background.jpg"
#else
#define default_width  1000
#define default_height 1000
#endif
#define image_update_time 0
#define SPACER

#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>

#include <fstream>

#include <chrono>
#include <mutex>



#define INCREASE(x)                      \
do {                                     \
	if ((x) == 0.0)                  \
	{                                \
		x = 1;                   \
	}                                \
	else if ((x) < 0.0)              \
	{                                \
		x = .9 * x;              \
	}                                \
	else                             \
	{                                \
		x = 1.1 * x;             \
	}                                \
} while(0)

#define DECREASE(x)                      \
do {                                     \
	if ((x) == 0.0)                  \
	{                                \
		x = 1;                   \
	}                                \
	else if ((x) < 0.0)              \
	{                                \
		x = 1.1 * x;             \
	}                                \
	else                             \
	{                                \
		x = .9 * x;              \
	}                                \
} while(0)

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

	void get_bounds(const City* c, double&xmin, double&xmax, double&ymin, double&ymax)
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

	void print_sol_to_mat(const Solution* sol, state* istate)
	{
		double minx, miny, maxx, maxy;
		get_bounds(sol->get_city(), minx, maxx, miny, maxy);
		DECREASE(minx);
		DECREASE(miny);
		INCREASE(maxx);
		INCREASE(maxy);

		// std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count()

		for (int d=0;d<sol->get_num_drivers();d++)
		{
			int length = sol->get_number_of_stops(d);
			if (length <= 0)
			{
				continue;
			}
			cv::Scalar& color = istate->get_color(d);

			// draw first line
			Coord& c = sol->get_city()->coords[sol->get_city()->start_location];
			double x = istate->mat.cols * (c.x - minx) / (maxx - minx);
			double y = istate->mat.rows * (c.y - miny) / (maxy - miny);
			cv::Point prev((int) x, (int) y);
			cv::circle(istate->mat, prev, 3, color);

			// draw middle lines
			for (int s = 0; s < length; s++)
			{
				int loc = sol->get_city()->get_action(sol->get_action_index(d, s)).location;
				x = istate->mat.cols * (sol->get_city()->coords[loc].x - minx) / (maxx - minx);
				y = istate->mat.rows * (sol->get_city()->coords[loc].y - miny) / (maxy - miny);

				cv::Point next((int) x, (int) y);
				cv::circle(istate->mat, next, 3, color);

				if ((prev.x == 0 && prev.y == 0) ||
						(next.x == 0 && next.y == 0))
				{
					err() << *sol->get_city() << std::endl;
					err() << x << ", " << y << std::endl;
					err() << "wth?" << std::endl;

					trap();
				}

				cv::line(istate->mat, prev, next, color, 1, 8, 0);

				prev = next;
			}

			// draw last line
			c = sol->get_city()->coords[sol->get_city()->start_location];
			x = istate->mat.cols * (c.x - minx) / (maxx - minx);
			y = istate->mat.rows * (c.y - miny) / (maxy - miny);
			cv::Point next((int) x, (int) y);
			cv::line(istate->mat, prev, next, color, 1, 8, 0);
		}

		std::stringstream s;
		s << "n=" << sol->get_num_serviced() << " t=" << sol->sum_all_times();

		cv::Scalar invert = istate->get_color(sol->get_num_drivers() + 1);
		cv::putText(istate->mat, s.str(), cv::Point(0,20), CV_FONT_HERSHEY_PLAIN, 1.0, invert);
	}

	void print_city_to_mat(const City* c, state* istate)
	{
		double minx, miny, maxx, maxy;
		get_bounds(c, minx, maxx, miny, maxy);
		DECREASE(minx);
		DECREASE(miny);
		INCREASE(maxx);
		INCREASE(maxy);

		cv::Scalar& color = istate->get_color(0);

		for (int i = 0; i < c->num_locations; i++)
		{
			double x = istate->mat.cols * (c->coords[i].x - minx) / (maxx - minx);
			double y = istate->mat.rows * (c->coords[i].y - miny) / (maxy - miny);

			cv::Point prev((int) x, (int) y);
			cv::circle(istate->mat, prev, 3, color);

			prev.x = prev.x - 20;
			prev.y = prev.y - 10;
			cv::putText(istate->mat, c->get_decription(i), prev, CV_FONT_HERSHEY_PLAIN, 1.0, color);
		}
	}

	void write_trucks_to_mat(const Solution* sol, const Coord* coords, int *actions, int time, state* istate)
	{
		double minx, miny, maxx, maxy;
		get_bounds(sol->get_city(), minx, maxx, miny, maxy);
		DECREASE(minx);
		DECREASE(miny);
		INCREASE(maxx);
		INCREASE(maxy);

		for (int driver = 0; driver < sol->get_num_drivers(); driver++)
		{
			cv::Scalar& color = istate->get_color(driver);

			double x = istate->mat.cols * (coords[driver].x - minx) / (maxx - minx);
			double y = istate->mat.rows * (coords[driver].y - miny) / (maxy - miny);

			cv::Point prev((int) x, (int) y);

			// for now. Why will make it pretty later...
			cv::circle(istate->mat, prev, 5, color);

			std::stringstream ss;
			if (actions[driver] == -1)
			{
				ss << "N";
			}
			else
			{
				operation last_op = sol->get_city()->get_action(actions[driver]).op;
				if (last_op == Store || last_op == Dropoff)
				{
					ss << "N";
				}
				else if (last_op == Pickup || last_op == Replace)
				{
					ss << "F(" << sol->get_city()->get_action(actions[driver]).out << ")";
				}
				else
				{
					ss << "E(" << sol->get_city()->get_action(actions[driver]).out << ")";
				}
			}
			prev.x = prev.x - 20;
			prev.y = prev.y + 20;
			cv::putText(istate->mat, ss.str(), prev, CV_FONT_HERSHEY_PLAIN, 1.0, color);
		}

		std::stringstream ss;
		ss << "time=" << time;
		cv::putText(istate->mat, ss.str(), cv::Point(0, 40), CV_FONT_HERSHEY_PLAIN, 1.0, cv::Scalar{255,255,255});
	}
}

cvvizualizer::cvvizualizer(const std::string& name)
{
	internal_state = new state(name);
}

cvvizualizer::~cvvizualizer()
{
	delete get_state(internal_state);
}

void cvvizualizer::show(const Solution* sol)
{
	state* istate = get_state(internal_state);
	std::lock_guard<std::mutex> lock{istate->mut};
	clear_mat(istate);
	print_city_to_mat(sol->get_city(), istate);
	print_sol_to_mat(sol, istate);

	std::lock_guard<std::mutex> global{graphics_mutex};
	imshow(istate->name, istate->mat);
	write_frame_of_video();
	cv::waitKey(image_update_time);
}


void cvvizualizer::show(const Solution* sol, int time)
{
	state* istate = get_state(internal_state);
	std::lock_guard<std::mutex> lock{istate->mut};
	clear_mat(istate);
	print_city_to_mat(sol->get_city(), istate);
	print_sol_to_mat(sol, istate);

	Coord* coords = new Coord[sol->get_num_drivers()];
	int* actions = new int[sol->get_num_drivers()];

	take_still_shot(sol, time, coords, actions);
	write_trucks_to_mat(sol, coords, actions, time, istate);

	delete[] coords;
	delete[] actions;

	std::lock_guard<std::mutex> global{graphics_mutex};
	imshow(istate->name, istate->mat);
	write_frame_of_video();
	cv::waitKey(image_update_time);
}


void cvvizualizer::show(const City* c)
{
	state* istate = get_state(internal_state);
	std::lock_guard<std::mutex> lock{istate->mut};
	clear_mat(istate);
	print_city_to_mat(c, istate);

	std::lock_guard<std::mutex> global{graphics_mutex};
	imshow(istate->name, istate->mat);
	cv::waitKey(image_update_time);
}

void cvvizualizer::pause(int length)
{
	cv::waitKey(length);
}

void cvvizualizer::snapshot(std::string file)
{
	std::lock_guard<std::mutex> lock{get_state(internal_state)->mut};
	std::lock_guard<std::mutex> global{graphics_mutex};
	cv::imwrite(get_state(internal_state)->name, get_state(internal_state)->mat);
}

void cvvizualizer::write_frame_of_video()
{
	state* istate = get_state(internal_state);
// must be already locked...
//	std::lock_guard<std::mutex> lock{istate->mut};
	istate->vid.write(istate->mat);
}
