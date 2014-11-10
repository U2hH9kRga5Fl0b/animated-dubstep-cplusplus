/*
 * city.cpp
 *
 *  Created on: Oct 24, 2014
 *      Author: thallock
 */

#include "model/city.h"

#include "model/landfill.h"
#include "main/gen_points.h"

#include "model/picture_bounds.h"

#include <cstdlib>
#include <cmath>
#include <sstream>

#define DIST_SCALE 5000

// This doesn't work because there is no operation for 'replacing' a dumpster at a yard.
#define ALL_YARD_COMBOS 1

#include <set>


namespace
{
	inline int get_num_actions(int num_requests_, int num_landfills_, int num_stagingareas_)
	{
		return num_requests_ + NUM_ACTIONS_PER_FILL * num_landfills_ + NUM_ACTIONS_PER_YARD * num_stagingareas_ ;
	}
}

City::City(int num_requests_, int num_landfills_, int num_stagingareas_, int num_trucks_) :
	num_actions      {get_num_actions(num_requests_, num_landfills_, num_stagingareas_)},
	num_requests     {num_requests_     },
	num_landfills    {num_landfills_    },
	num_stagingareas {num_stagingareas_ },
	num_locations    {num_requests + num_landfills + num_stagingareas + 2 * num_trucks_},
	num_trucks       {num_trucks_       },
	durations        {num_locations     },
	possibles        {num_actions       },
	coords           {new Coord[num_locations]},
	trucks           {new truck_types[num_trucks]},
	begin_actions    {new Action[num_trucks_]},
	final_actions    {new Action[num_trucks_]},
	actions          {nullptr           },
	donttouch        {                  }

{
	gen_points(     &coords[0], num_landfills,
			&coords[num_landfills], num_stagingareas,
			&coords[num_landfills + num_stagingareas], num_requests + 2 * num_trucks_,
			UNIFORM_CITY_TYPE);

	int action_index = 0;

	int ndx = 0;
	for (int i = 0; i < num_landfills; i++)
	{
		Landfill l { ndx++ };
		donttouch.push_back(Action{l, six    , action_index++});
		donttouch.push_back(Action{l, nine   , action_index++});
		donttouch.push_back(Action{l, twelve , action_index++});
		donttouch.push_back(Action{l, sixteen, action_index++});
	}

	truck_types tt[] = {small, medium, large};
	for (int i = 0; i < num_trucks; i++)
		trucks[i] = tt[rand() % 3];

	for (int t = 0; t < num_stagingareas; t++)
	{
		yards.push_back(Yard{ndx++});

		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | six,     TRUCK_STATE_NONE, action_index++ });                // index:  0
		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | nine,    TRUCK_STATE_NONE, action_index++ });                // index:  1
		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | twelve,  TRUCK_STATE_NONE, action_index++ });                // index:  2
		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | sixteen, TRUCK_STATE_NONE, action_index++ });                // index:  3

		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_NONE, TRUCK_STATE_EMPTY | six      , action_index++});               // index:  4
		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_NONE, TRUCK_STATE_EMPTY | nine     , action_index++});               // index:  5
		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_NONE, TRUCK_STATE_EMPTY | twelve   , action_index++});               // index:  6
		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_NONE, TRUCK_STATE_EMPTY | sixteen  , action_index++});               // index:  7

		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | six      , TRUCK_STATE_EMPTY | nine     , action_index++});  // index:  8
		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | six      , TRUCK_STATE_EMPTY | twelve   , action_index++});  // index:  9
		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | six      , TRUCK_STATE_EMPTY | sixteen  , action_index++});  // index: 10

		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | nine     , TRUCK_STATE_EMPTY | six      , action_index++});  // index: 11
		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | nine     , TRUCK_STATE_EMPTY | twelve   , action_index++});  // index: 12
		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | nine     , TRUCK_STATE_EMPTY | sixteen  , action_index++});  // index: 13

		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | twelve   , TRUCK_STATE_EMPTY | six      , action_index++});  // index: 14
		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | twelve   , TRUCK_STATE_EMPTY | nine     , action_index++});  // index: 15
		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | twelve   , TRUCK_STATE_EMPTY | sixteen  , action_index++});  // index: 16

		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | sixteen  , TRUCK_STATE_EMPTY | six      , action_index++});  // index: 17
		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | sixteen  , TRUCK_STATE_EMPTY | nine     , action_index++});  // index: 18
		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | sixteen  , TRUCK_STATE_EMPTY | twelve   , action_index++});  // index: 19
	}

	for (int i = 0; i < num_requests; i++)
		donttouch.push_back(Action{ndx++, action_index++});


	for (int i = 0; i < num_trucks; i++)
	{
		begin_actions[i].location = ndx++;
		final_actions[i].location = ndx++;
	}

	if (ndx != num_locations)
	{
		std::cerr << "wrong number of locations!" << std::endl;
		exit(-1);
	}

	common_init();
}


City::City(const City* other, bool *requests_completed, int count, Coord *locations, truck_state* states) :
	num_actions      {get_num_actions(other->num_requests - count, other->num_landfills, other->num_stagingareas)},
	num_requests     {other->num_requests - count},
	num_landfills    {other->num_landfills},
	num_stagingareas {other->num_stagingareas},
	num_locations    {other->num_locations - count},
	num_trucks       {other->num_trucks},
	durations        {num_locations},
	possibles        {num_actions},
	coords           {new Coord[num_locations]},
	trucks           {new truck_types[num_trucks]},
	begin_actions    {new Action[num_trucks]},
	final_actions    {new Action[num_trucks]},
	actions          { nullptr },
	donttouch	 { }
{
	for (int i = 0; i < other->first_request_index; i++)
	{
		donttouch.push_back(Action { other->get_action(i) });
	}
	for (int i = 0; i < num_landfills + num_stagingareas; i++)
		coords[i] = other->coords[i];

	int location_index = num_landfills + num_stagingareas;

	for (int i = 0; i < other->num_requests; i++)
	{
		int action_index = other->first_request_index + i;
		if (requests_completed[i])
			continue;
		donttouch.push_back(Action { other->get_action(action_index) });
		coords[location_index] = other->coords[other->get_action(action_index).location];
		donttouch.back().location = location_index++;
		donttouch.back().idx = donttouch.size() - 1;
	}

	for (int i = 0; i < other->num_trucks; i++)
	{
		trucks[i] = other->trucks[i];

		for (int j = 0; j < i; j++)
			truck_name(trucks[j]);
		truck_name(trucks[i]);

		begin_actions[i] = other->begin_actions[i];
		begin_actions[i].location -= count;
		final_actions[i] = other->final_actions[i];
		final_actions[i].location -= count;

		begin_actions[i].exit_state = states[i];
		coords[begin_actions[i].location] = locations[i];
		coords[final_actions[i].location] = other->coords[other->final_actions[i].location];
	}

	yards = other->yards;

	common_init();
}

void City::common_init()
{
	double avg_drive_time = 0.0;
	for (int i = 0; i < num_locations; i++)
	for (int j = 0; j < num_locations; j++)
		avg_drive_time += durations.at(i,j) = (int)(DIST_SCALE * coords[i].dist(coords[j]));

	log() << "average drive time: " << (avg_drive_time / (60.0 * num_locations * (num_locations - 1))) << " minutes" << std::endl;

	actions = donttouch.data();
	if (num_actions != (int) donttouch.size())
	{
		err() << "wrong number of actions!" << std::endl;
		err() << "expected=" << num_actions << " number of actions=" << donttouch.size() << std::endl;
		trap();
	}

	int avg_num_possibles = 0;
	possibles = -1;
	for (int i=0; i<num_actions; i++)
	{
		int ndx=0;
		for (int j=num_actions-1; j>=0; j--)
		{
			if (actions[j].entr_state != actions[i].exit_state)
			{
				continue;
			}
			if (actions[i].begin_time > actions[j].end_time)
			{
				continue;
			}
			possibles.at(i, ndx++) = j;
			avg_num_possibles++;
		}
	}
	log() << "average number of possibles actions: " << (avg_num_possibles / num_actions) << std::endl;

	first_landfill_index    = 0;
	first_stagingarea_index = first_landfill_index + num_landfills * NUM_ACTIONS_PER_FILL;
	first_request_index     = first_stagingarea_index + num_stagingareas * NUM_ACTIONS_PER_YARD;

	xmin = ymin =  DBL_MAX;
	xmax = ymax = -DBL_MAX;
	for (int i = 0; i < num_locations; i++)
	{
		if (coords[i].x < xmin)
		{
			xmin = coords[i].x;
		}
		if (coords[i].x > xmax)
		{
			xmax = coords[i].x;
		}
		if (coords[i].y < ymin)
		{
			ymin = coords[i].y;
		}
		if (coords[i].y > ymax)
		{
			ymax = coords[i].y;
		}
	}

	DECREASE(xmin);
	DECREASE(ymin);
	INCREASE(xmax);
	INCREASE(ymax);
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
				break;
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
		for (int i = 0; i < num_trucks; i++)
		{
			if (begin_actions[i].location == location)
			{
				std::stringstream ss;
				ss << "start_" << i;
				return ss.str();
			}
			if (final_actions[i].location == location)
			{
				std::stringstream ss;
				ss << "stop_" << i;
				return ss.str();
			}
		}

		std::cerr << "unable to find an action for location " << location << std::endl;
		trap();
	}


	if (is_landfill(anaction))
	{
		return "L";
	}
	else if (is_staging_area(anaction))
	{
		return "Y";
	}
	else
	{
		std::stringstream ss;
		ss << (actions[anaction].entr_state & TRUCK_SIZE_MASK) << "->" << (actions[anaction].exit_state & TRUCK_SIZE_MASK);
		return ss.str();
	}
}
