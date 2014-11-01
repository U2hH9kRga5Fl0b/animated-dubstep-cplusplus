/*
 * city.cpp
 *
 *  Created on: Oct 24, 2014
 *      Author: thallock
 */

#include "model/city.h"

#include "model/landfill.h"
#include "main/gen_points.h"

#include <cstdlib>
#include <cmath>
#include <sstream>

#define DIST_SCALE 5000



// This doesn't work because there is no operation for 'replacing' a dumpster at a yard.
#define ALL_YARD_COMBOS 0

#include <set>

City::City(int num_requests_, int num_landfills_, int num_stagingareas_, int num_trucks_) :
#if ALL_YARD_COMBOS
	num_actions      {num_requests_+4*num_landfills_+4*5*2*num_stagingareas_},
#else
	num_actions      {num_requests_+4*num_landfills_+4*2*num_stagingareas_},
#endif
	num_requests     {num_requests_     },
	num_landfills    {num_landfills_    },
	num_stagingareas {num_stagingareas_ },
	num_locations    {num_requests + num_landfills + num_stagingareas},
	num_trucks       {num_trucks_       },
	start_location   {0                 },
	durations        {num_locations     },
	possibles        {num_actions       },
	coords           {new Coord[num_locations]},
	trucks           {new truck_types[num_trucks]},
	actions          {nullptr           },
	donttouch        {                  }
{
	int ndx = 0;
	gen_points(     &coords[0], num_landfills,
			&coords[num_landfills], num_stagingareas,
			&coords[num_landfills + num_stagingareas], num_requests,
			CLUSTER_CITY_TYPE);

	for (int i = 0; i < num_landfills; i++)
	{
		Landfill l { ndx++ };
		donttouch.push_back(Action{l, six    });
		donttouch.push_back(Action{l, nine   });
		donttouch.push_back(Action{l, twelve });
		donttouch.push_back(Action{l, sixteen});
	}

	truck_types tt[] = {small, medium, large};
	for (int i=0;i<num_trucks;i++)
	{
		trucks[i] = tt[rand() % 3];
	}

	for (int t = 0; t < num_stagingareas; t++)
	{
		yards.push_back(Yard{ndx++});
#if ALL_YARD_COMBOS
		dumpster_size sizes[] = {none, six, nine, twelve, sixteen};
		operation ops[] = {Store, Unstore};
		for (int i=0;i<5;i++)
		for (int j=0;j<5;j++)
		for (int k=0;k<2;k++)
		{
			if (i==j) continue;
			donttouch.push_back(Action{y, sizes[i], sizes[j], ops[k]});
		}
#else
		dumpster_size sizes[] = {six, nine, twelve, sixteen};
		for (int i=0;i<4;i++)
		{
			donttouch.push_back(Action{yards.at(t), sizes[i], none, Store});
			donttouch.push_back(Action{yards.at(t), none, sizes[i], Unstore});
		}
#endif
	}

	for (int i = 0; i < num_requests; i++)
		donttouch.push_back(Action{ndx++});

	if (ndx != num_locations)
	{
		std::cerr << "wrong number of locations!" << std::endl;
		exit(-1);
	}

	double avg_drive_time = 0.0;
	for (int i = 0; i < num_locations; i++)
	for (int j = 0; j < num_locations; j++)
	{
		avg_drive_time += durations.at(i,j) = (int)(DIST_SCALE * coords[i].dist(coords[j]));
	}
	log() << "average drive time: " << (avg_drive_time / (60.0 * num_locations * (num_locations - 1))) << " minutes" << std::endl;

	actions = donttouch.data();
	if (num_actions != (int) donttouch.size())
	{
		std::cerr << "wrong number of actions!" << std::endl;
		std::cerr << "expected=" << num_actions << " number of actions=" << donttouch.size() << std::endl;
		exit(-1);
	}

	int avg_num_possibles = 0;
	possibles = -1;
	for (int i=0; i<num_actions; i++)
	{
		int ndx=0;
		for (int j=num_actions-1; j>=0; j--)
		{
			if (!op_follows_op[actions[j].op][actions[i].op])
			{
				continue;
			}
			if (actions[i].begin_time > actions[j].end_time)
			{
				continue;
			}
			if (actions[i].out != actions[j].in)
			{
				continue;
			}
			possibles.at(i, ndx++) = j;
			avg_num_possibles++;
		}
	}
	log() << "average number of possibles actions: " << (avg_num_possibles / num_actions) << std::endl;

	for (int i = 0; i < num_actions; i++)
	{
		if (actions[i].op == Unstore || actions[i].op == Store)
		{
			start_location = actions[i].location;
			break;
		}
	}
}

City::~City()
{
	delete[] coords;
	delete[] trucks;
}



std::ostream& operator<<(std::ostream& out, const City& c)
{
	out << "actions:\n";
	for (int i = 0; i < c.num_actions; i++)
	{
		out << std::setw(4) << i << ':' << c.actions[i] << std::endl;
	}
	out << "possibles:\n";
	for (int i = 0; i < c.num_actions; i++)
	{
		out << std::setw(5) << i << ':';
		for (int j = 0; j < c.num_actions; j++)
		{
			if (c.possibles.at(i, j) < 0)
			{
				break;
			}
			out << std::setw(3) << c.possibles.at(i, j) << ' ';
		}
		out << std::endl;
	}
	out << "trucks:\n";
	for (int i = 0; i < c.num_trucks; i++)
	{
		out << std::setw(4) << i << ':' << truck_name(c.trucks[i]) << std::endl;
	}

	out << "locs:\n";
	for (int i = 0; i < c.num_locations; i++)
	{
		out << "\t" << std::setw(3) << i << ":" << std::setw(8) << c.coords[i].x << ", " << std::setw(8) << c.coords[i].y << "\n";
	}

	out << "distances:\n";
	out << c.durations << std::endl;
	return out;
}

std::string City::get_decription(int location) const
{
	INBOUNDS(0, location, num_locations);
	int anaction = -1;

	for (int i = 0; i < num_actions; i++)
	{
		if (actions[i].location == location)
		{
			anaction = i;
			break;
		}
	}
	if (anaction < 0)
	{
		std::cerr << "unable to find an action for location " << location << std::endl;
		trap();
	}

	operation op = actions[anaction].op;
	if (op == Dump)
	{
		return "L";
	}
	else if (op == Unstore || op == Store)
	{
		return "Y";
	}
	else
	{
		std::stringstream ss;
		ss << actions[anaction].in << "->" << actions[anaction].out;
		return ss.str();
	}
}
