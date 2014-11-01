/*
 * gen_points.cpp
 *
 *  Created on: Oct 31, 2014
 *      Author: thallock
 */


#include "main/gen_points.h"
#include "common.h"

#include <random>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cmath>

double cone_factor = .5;
double cone_start = 10 * rand() / (double) RAND_MAX;
double PI = std::atan(1.0)*4;


void gen_points(Coord* landfills, int num_fills,
		Coord* yards,     int num_yards,
		Coord* requests,  int num_requests,
		points_pattern pattern)
{

	switch (pattern)
	{
		case UNIFORM_CITY_TYPE:
		{
			for (int i = 0; i < num_fills; i++)
			{
				landfills[i].x = rand() / (double) RAND_MAX;
				landfills[i].y = rand() / (double) RAND_MAX;
			}

			for (int i = 0; i < num_yards; i++)
			{
				if (STAGING_AREAS_GO_WITH_LANDFILLS)
				{
					yards[i] = landfills[i % num_fills];
				}
				else
				{
					yards[i].x = rand() / (double) RAND_MAX;
					yards[i].y = rand() / (double) RAND_MAX;
				}
			}
			for (int i = 0; i < num_requests; i++)
			{
				requests[i].x = rand() / (double) RAND_MAX;
				requests[i].y = rand() / (double) RAND_MAX;
			}
		}
			break;
		case CIRCLE_CITY_TYPE:
		{
			for (int i = 0; i < num_fills; i++)
			{
				double radius = .9 + .2 * (rand() / (double) RAND_MAX);
				double theta = cone_start + 2 * PI * cone_factor * (rand() / (double) RAND_MAX);
				landfills[i].x = radius * std::cos(theta);
				landfills[i].y = radius * std::sin(theta);
			}
			for (int i = 0; i < num_yards; i++)
			{
				if (STAGING_AREAS_GO_WITH_LANDFILLS)
				{
					yards[i] = landfills[i % num_fills];
				}
				else
				{
					double radius = .9 + .2 * (rand() / (double) RAND_MAX);
					double theta = cone_start + 2 * PI * cone_factor * (rand() / (double) RAND_MAX);
					yards[i].x = radius * std::cos(theta);
					yards[i].y = radius * std::sin(theta);
				}
			}
			for (int i = 0; i < num_requests; i++)
			{
				double radius = .9 + .2 * (rand() / (double) RAND_MAX);
				double theta = cone_start + 2 * PI * cone_factor * (rand() / (double) RAND_MAX);
				requests[i].x = radius * std::cos(theta);
				requests[i].y = radius * std::sin(theta);
			}
		}
			break;
		case CLUSTER_CITY_TYPE:
		{
			for (int i = 0; i < num_fills; i++)
			{
				landfills[i].x = 15 * (rand() / (double) RAND_MAX);
				landfills[i].y = 15 * (rand() / (double) RAND_MAX);
			}
			for (int i = 0; i < num_yards; i++)
			{
				yards[i] = landfills[i % num_fills];
			}

			for (int i = 0; i < num_requests; i++)
			{
				double radius = rand() / (double) RAND_MAX;
				double theta = 2 * PI * (rand() / (double) RAND_MAX);

				int fill = rand() % num_fills;

				requests[i].x = landfills[fill].x + radius * std::cos(theta);
				requests[i].y = landfills[fill].y + radius * std::sin(theta);
			}
		}
			break;
		case RADIAL_CITY_TYPE:
		{
			std::default_random_engine generator;
			std::normal_distribution<double> distribution(0.0, 1.0);

			for (int i = 0; i < num_fills; i++)
			{
				double radius = distribution(generator);
				double theta = cone_start + 2 * PI * cone_factor * (rand() / (double) RAND_MAX);
				landfills[i].x = radius * std::cos(theta);
				landfills[i].y = radius * std::sin(theta);
			}
			for (int i = 0; i < num_yards; i++)
			{
				if (STAGING_AREAS_GO_WITH_LANDFILLS)
				{
					yards[i] = landfills[i % num_fills];
				}
				else
				{
					double radius = distribution(generator);
					double theta = cone_start + 2 * PI * cone_factor * (rand() / (double) RAND_MAX);
					yards[i].x = radius * std::cos(theta);
					yards[i].y = radius * std::sin(theta);
				}
			}
			for (int i = 0; i < num_requests; i++)
			{
				double radius = distribution(generator);
				double theta = cone_start + 2 * PI * cone_factor * (rand() / (double) RAND_MAX);
				requests[i].x = radius * std::cos(theta);
				requests[i].y = radius * std::sin(theta);
			}
		}
			break;
		case SPIRAL_CITY_TYPE:
		{
			constexpr int scale = 50;
			constexpr double out = .75;

			for (int i = 0; i < num_fills; i++)
			{
				double t = scale * (rand() / (double) (RAND_MAX));
				landfills[i].x = t * std::cos(t / out);
				landfills[i].y = t * std::sin(t / out);
			}
			for (int i = 0; i < num_yards; i++)
			{
				if (STAGING_AREAS_GO_WITH_LANDFILLS)
				{
					yards[i] = landfills[i % num_fills];
				}
				else
				{
					double t = scale * (rand() / (double) (RAND_MAX));
					yards[i].x = t * std::cos(t / out);
					yards[i].y = t * std::sin(t / out);
				}
			}
			for (int i = 0; i < num_requests; i++)
			{
				double t = scale * (rand() / (double) (RAND_MAX));
				requests[i].x = t * std::cos(t / out);
				requests[i].y = t * std::sin(t / out);
			}
		}
		break;
	}
}
