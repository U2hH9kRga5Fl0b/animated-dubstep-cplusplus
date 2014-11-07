/*
 * insight_viewer.cpp
 *
 *  Created on: Nov 6, 2014
 *      Author: thallock
 */

#include "insight/insight_viewer.h"


#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include "viz/viewer.h"

#define default_height 500
#define default_width  500





namespace
{

bool init;

std::string get_size(truck_state s)
{
	std::stringstream ss;
	ss << (int) (s & TRUCK_SIZE_MASK);
	return ss.str();
}
std::string get_deliver_size(const City* city, int action)
{
	return get_size(city->get_action(action).entr_state);
}
std::string get_pickup_size(const City* city, int action)
{
	return get_size(city->get_action(action).exit_state);
}


void draw_line(cv::Mat& canvas, const City *city, int begin_location, int end_location, cv::Scalar color, const std::string& text)
{
	const double minx = city->xmin;
	const double miny = city->ymin;
	const double maxx = city->xmax;
	const double maxy = city->ymax;

	cv::Point prev( (int) canvas.cols * (city->coords[begin_location].x - minx) / (maxx - minx),
			(int) canvas.rows * (city->coords[begin_location].y - miny) / (maxy - miny));

	cv::Point next( (int) canvas.cols * (city->coords[end_location].x - minx) / (maxx - minx),
			(int) canvas.rows * (city->coords[end_location].y - miny) / (maxy - miny));

	//	std::stringstream s;
	//	s << "n=" << sol->get_num_serviced() << " t=" << sol->sum_all_times();

	cv::putText(canvas, text, cv::Point{next.x + 5, next.y}, CV_FONT_HERSHEY_PLAIN, 1.0, color);
	cv::circle(canvas, next, 3, color);
	cv::line(canvas, prev, next, color, 1, 8, 0);
}

}

void show_insight(const std::string& name, const insight_state* state)
{
	const City* city = state->info->city;

	cv::Mat canvas = cv::Mat::zeros(default_height, default_width, CV_8UC3);

	if (!init)
	{
		cvNamedWindow(name.c_str(), CV_WINDOW_AUTOSIZE);
		cvMoveWindow(name.c_str(), 100,100);
		init = true;
	}


	// depots associated to delivers
	for (int d = 0; d < state->info->num_delivers; d++)
	{
		int da = state->info->deliver_actions[d];
		int dloc = city->get_action(da).location;
		int ddloc = city->yards.at(state->deliver_depots[d]).location;
		draw_line(canvas, city, ddloc, dloc, cv::Scalar { 255, 0, 0 }, get_deliver_size(city, da));
	}

	// depots associated to pickups
	for (int p = 0; p < state->info->num_pickups; p++)
	{
		int pa = state->info->pickup_actions[p];
		int ploc = city->get_action(pa).location;
		int pdloc = city->yards.at(state->pickup_depots[p]).location;
		draw_line(canvas, city, pdloc, ploc, cv::Scalar { 0, 255, 0 }, get_pickup_size(city, pa));
	}

	// pickups associated to delivers
	for (int d = 0; d < state->info->num_delivers; d++)
	{
		int da = state->info->deliver_actions[d];
		int pi = state->delivers_to_pickups[d];
		if (pi < 0)
		{
			continue;
		}
		int pa = state->info->pickup_actions[pi];

		int dloc = city->get_action(da).location;
		int ploc = city->get_action(pa).location;

		draw_line(canvas, city, dloc, ploc, cv::Scalar { 0, 0, 255 }, "");
	}

	cv::imshow(name, canvas);
	cv::waitKey(500);
}

