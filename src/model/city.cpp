/*
 * city.cpp
 *
 *  Created on: Oct 24, 2014
 *      Author: thallock
 */

#include "model/city.h"

#include <cstdlib>
#include <cmath>

#define DIST_SCALE 500



// This doesn't work because there is no operation for 'replacing' a dumpster at a yard.
#define ALL_YARD_COMBOS 0

#include <set>

Coord::Coord(double x_, double y_) : x{x_}, y{y_} {}
Coord::Coord(const Coord& other) : Coord{other.x, other.y} {}
Coord::Coord() : Coord{rand() / (double) RAND_MAX, rand() / (double) RAND_MAX} {}
Coord::~Coord() {}
double Coord::dist(const Coord& other) const { double dx = other.x-x; double dy=other.y-y; return sqrt(dx*dx+dy*dy);}


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
	actions          {nullptr           },
	durations        {num_locations     },
	possibles        {num_actions       },
	coords           {new Coord[num_locations]},
	trucks           {new truck_types[num_trucks]},
	donttouch        {                  }
{
	int ndx = 0;

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
		Yard y {ndx++};
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
			donttouch.push_back(Action{y, sizes[i], none, Store});
			donttouch.push_back(Action{y, none, sizes[i], Unstore});
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

	for (int i = 0; i < num_locations; i++)
	for (int j = 0; j < num_locations; j++)
		durations.at(i,j) = (int)(DIST_SCALE * coords[i].dist(coords[j]));

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
		for (int j=0; j<num_actions; j++)
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
	std::cout << "average number of possibles actions: " << (avg_num_possibles / num_actions) << std::endl;
}

City::~City()
{
	delete[] coords;
	delete[] trucks;
}



std::ostream& operator<<(std::ostream& out, const City& c)
{
	out << "actions:\n";
	for (int i=0;i<c.num_actions;i++)
	{
		out << std::setw(4) << i << ':' << c.actions[i] << std::endl;
	}
	out << "trucks:\n";
	for (int i=0;i<c.num_trucks;i++)
	{
		out << std::setw(4) << i << ':' << truck_name(c.trucks[i]) << std::endl;
	}

	if (c.num_locations < 50)
	{
		out << "distances:\n";
		out << c.durations << std::endl;
	}
	return out;
}
