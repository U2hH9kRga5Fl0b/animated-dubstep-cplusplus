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
#define image_update_time 20
#define SPACER

#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>

#include <fstream>

#include <chrono>
#include <mutex>

namespace
{
	constexpr int thickness = 2;
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

			colors.push_back(cv::Scalar {   0,   0, 150 });
			colors.push_back(cv::Scalar {   0, 150,   0 });
			colors.push_back(cv::Scalar { 150,   0,   0 });
			colors.push_back(cv::Scalar { 150, 150,   0 });
			colors.push_back(cv::Scalar { 150,   0, 150 });
			colors.push_back(cv::Scalar {   0, 150, 150 });
			colors.push_back(cv::Scalar { 150,  10,  10 });
			colors.push_back(cv::Scalar {   0,  10, 150 });
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

	void clear_mat(state* istate)
	{
#if USE_BACKGROUND_IMAGE
		istate->mat = cv::imread(BACKGROUND_IMAGE, CV_LOAD_IMAGE_COLOR);
#else
		istate->mat = cv::Scalar(255, 255, 255);
#endif
	}

	void print_sol_to_mat(const Solution* sol, state* istate)
	{
		const double minx = sol->get_city()->xmin;
		const double miny = sol->get_city()->ymin;
		const double maxx = sol->get_city()->xmax;
		const double maxy = sol->get_city()->ymax;

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
			Coord c = sol->get_city()->get_start_location(d);
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

//					trap();
				}

				cv::line(istate->mat, prev, next, color, thickness, 8, 0);

				prev = next;
			}

			// draw last line
			c = sol->get_city()->get_end_location(d);
			x = istate->mat.cols * (c.x - minx) / (maxx - minx);
			y = istate->mat.rows * (c.y - miny) / (maxy - miny);
			cv::Point next((int) x, (int) y);
			cv::line(istate->mat, prev, next, color, thickness, 8, 0);
		}

		std::stringstream s;
		s << "n=" << sol->get_num_serviced() << " t=" << sol->sum_all_times();

		cv::Scalar invert = istate->get_color(sol->get_num_drivers() + 1);
		cv::putText(istate->mat, s.str(), cv::Point(0,20), CV_FONT_HERSHEY_PLAIN, 1.0, invert);
	}

	void print_city_to_mat(const City* c, state* istate)
	{
		const double minx = c->xmin;
		const double miny = c->ymin;
		const double maxx = c->xmax;
		const double maxy = c->ymax;

		cv::Scalar& color = istate->get_color(0);

		for (int i = 0; i < c->num_locations; i++)
		{
			double x = istate->mat.cols * (c->coords[i].x - minx) / (maxx - minx);
			double y = istate->mat.rows * (c->coords[i].y - miny) / (maxy - miny);

			cv::Point prev((int) x, (int) y);
			cv::circle(istate->mat, prev, 3, color);

			prev.x = prev.x - 20;
			prev.y = prev.y - 10;
			std::string desc{c->get_decription(i)};
			if (desc.at(0) == 's' && desc.at(1) == 't' && desc.at(2) == 'o')
			{
				// hack to make sure start and stop are not printed on top of each other.
				prev.y += 30;
			}

			cv::putText(istate->mat, c->get_decription(i), prev, CV_FONT_HERSHEY_PLAIN, 1.0, color);
		}
	}

	void write_trucks_to_mat(const Solution* sol, const Coord* coords, int *actions, int time, state* istate)
	{
		const double minx = sol->get_city()->xmin;
		const double miny = sol->get_city()->ymin;
		const double maxx = sol->get_city()->xmax;
		const double maxy = sol->get_city()->ymax;

		for (int driver = 0; driver < sol->get_num_drivers(); driver++)
		{
			cv::Scalar& color = istate->get_color(driver);

			double x = istate->mat.cols * (coords[driver].x - minx) / (maxx - minx);
			double y = istate->mat.rows * (coords[driver].y - miny) / (maxy - miny);

			cv::Point prev((int) x, (int) y);

			// for now. Why will make it pretty later...
			cv::circle(istate->mat, prev, 5, color);

			prev.x = prev.x - 20;
			prev.y = prev.y + 20;
			cv::putText(istate->mat, get_truck_state_desc(sol->get_city()->get_action(actions[driver]).exit_state), prev, CV_FONT_HERSHEY_PLAIN, 1.0, color);
		}

		std::stringstream ss;
		ss << "time=" << time;
		cv::putText(istate->mat, ss.str(), cv::Point(0, 40), CV_FONT_HERSHEY_PLAIN, 1.0, cv::Scalar{255,255,255});
	}

	void print_directions_to_mat(const directions& dirs, state* istate)
	{
		// generate bounds:
		// 39.558185,-105.226243
		// 40.211516,-104.745591
//		double minx = 39.558185;
//		double maxx = 40.211516;
//		double miny = -105.226243;
//		double maxy = -104.745591;

		// image bounds:
		// 40.073125, -104.645848
		// 39.405639, -105.473585
		double minx = 39.405639;
		double maxx = 40.073125;
		double miny = -105.473585;
		double maxy = -104.645848;

		istate->mat = istate->mat = cv::imread("BailyToHudson.png", CV_LOAD_IMAGE_COLOR);

		cv::Scalar& color = istate->get_color(0);
		cv::Point prev(-1, -1);

		auto oend = dirs.steps.end();
		for (auto oit = dirs.steps.begin(); oit != oend; ++oit)
		{
			auto iend = oit->path.end();
			for (auto iit = oit->path.begin(); iit != iend; ++iit)
			{
				double x = istate->mat.cols * (iit->lat - minx) / (maxx - minx);
				double y = istate->mat.rows * (iit->lng - miny) / (maxy - miny);

//				y = istate->mat.rows - y;
				x = istate->mat.cols - x;

				cv::Point next((int) y, (int) x);
				cv::circle(istate->mat, next, 3, color);
				if (prev.x != -1 && prev.y != -1)
				{
					cv::line(istate->mat, prev, next, color, thickness, 8, 0);
				}
				next = prev;
			}
		}
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


void cvvizualizer::show(const directions& dirs)
{
	state* istate = get_state(internal_state);
	std::lock_guard<std::mutex> lock{istate->mut};
	clear_mat(istate);
	print_directions_to_mat(dirs, istate);

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
	cv::imwrite("output/" + file + ".png", get_state(internal_state)->mat);
}

void cvvizualizer::write_frame_of_video()
{
	if(true) return;
	state* istate = get_state(internal_state);
// must be already locked...
//	std::lock_guard<std::mutex> lock{istate->mut};
	istate->vid.write(istate->mat);
}
